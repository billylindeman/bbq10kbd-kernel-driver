#include "bbq10kbd.h"

#define INTERRUPT_GPIO 451

static int bbq10kbd_init_input_keyboard(struct bbq10kbd_keypad* keypad_data)
{
  struct input_dev* input;
  int ret;
  const int num_keycodes = ARRAY_SIZE(bbq10kbd_keycodes);

  printk(KERN_DEBUG "bbq10kbd: initialising internal input...");
  input = input_allocate_device();
  if(input == NULL)
    return -ENOMEM;
 
  input->name = "bbq10kbd-i2c-keyboard";

  input->evbit[0] = BIT_MASK(EV_KEY) ;
  input->keycode = bbq10kbd_keycodes;
  input->keycodesize = sizeof(unsigned short); 
  input->keycodemax = num_keycodes;

  for(ret = 0; ret < num_keycodes; ret++)
  {
    if(bbq10kbd_keycodes[ret] != KEY_RESERVED)
      set_bit(bbq10kbd_keycodes[ret], input->keybit);
  }

  ret = input_register_device(input);
  if(ret != 0)
  {
    printk(KERN_ERR "bbq10kbd: unable to register input device, register returned %d\n", ret);
    input_unregister_device(input);
    return -ENODEV;
  }

  printk(KERN_DEBUG "bbq10kbd: initialised input device with %d keycodes", num_keycodes);
  keypad_data->input_keyboard = input;

  return ret;

}

static int bbq10kbd_init_input_pointer(struct bbq10kbd_keypad* keypad_data)
{
  struct input_dev* input;
  int ret;

  printk(KERN_DEBUG "bbq10kbd: initialising internal input...");
  input = input_allocate_device();
  if(input == NULL)
    return -ENOMEM;
 
  input->name = "bbq10kbd-i2c-mouse";

  set_bit(INPUT_PROP_POINTER, input->propbit);
  set_bit(EV_REL, input->evbit);
  set_bit(EV_KEY, input->evbit);
  set_bit(REL_X, input->relbit);
  set_bit(REL_Y, input->relbit);
  set_bit(BTN_LEFT, input->keybit);
  set_bit(BTN_RIGHT, input->keybit);
  set_bit(REL_HWHEEL, input->relbit);

  // Configure keycodes
  ret = input_register_device(input);
  if(ret != 0)
  {
    printk(KERN_ERR "bbq10kbd: unable to register input device, register returned %d\n", ret);
    input_unregister_device(input);
    return -ENODEV;
  }

  printk(KERN_DEBUG "bbq10kbd: initialised input pointer");
  keypad_data->input_pointer = input;

  return ret;

}


static void bbq10kbd_irq_handle_key(struct bbq10kbd_keypad *keypad_data) {
    unsigned int fifo_read;
    unsigned char key_code, key_state;
    unsigned char evt_code;

    // Read REG_FIF until it's empty
    do {
        fifo_read = i2c_smbus_read_word_data(keypad_data->i2c, REG_FIF); 
        key_code = (fifo_read & 0xFF00) >> 8;
        key_state = fifo_read & 0x00FF;

        printk(KERN_DEBUG "bbq10kbd: handle_key fifo-read %02X, key: %02X, state: %d", fifo_read, key_code, key_state);

        if(key_state == KEY_PRESSED || key_state == KEY_RELEASED) {
            evt_code = bbq10kbd_keycodes[key_code];
            printk(KERN_DEBUG "bbq10kbd: input-event EV_KEY %d", evt_code);
            input_event(keypad_data->input_keyboard, EV_KEY, evt_code, (key_state == KEY_PRESSED));  
            input_sync(keypad_data->input_keyboard);
        }
    } while(fifo_read != 0x0000);

}

static void bbq10kbd_irq_handle_trackpad(struct bbq10kbd_keypad *keypad_data) {
    signed char tox, toy;

    // Read X/Y relative motion registers
    tox = (signed char) i2c_smbus_read_word_data(keypad_data->i2c, REG_TOX); 
    toy = (signed char) i2c_smbus_read_word_data(keypad_data->i2c, REG_TOY); 

    printk(KERN_DEBUG "bbq10kbd: handle_trackpad (x,y) = (%i,%i)", tox,  toy);

    // Send Relative Motion event to input device
    //
    input_event(keypad_data->input_pointer, EV_REL, REL_X, tox);
    input_event(keypad_data->input_pointer, EV_REL, REL_Y, toy);
    input_sync(keypad_data->input_pointer);
}


// IRQ Handler to read i2c fifo and generate events 
static irqreturn_t bbq10kbd_irq_handler(int irq, void *dev_id)
{
	struct bbq10kbd_keypad *keypad_data = dev_id;
    int error;
    unsigned char irq_reg;

    // Read Interrupt Status
    irq_reg = i2c_read_byte(keypad_data->i2c, REG_INT);

    printk(KERN_DEBUG "bbq10kbd: irq handler reg_int=%02x", irq_reg);

    // Dispatch interrupt event handler
    if (irq_reg & MASK_INT_KEY) {
        bbq10kbd_irq_handle_key(keypad_data);
    }else if (irq_reg & MASK_INT_TOUCH) {
        bbq10kbd_irq_handle_trackpad(keypad_data);
    }

    // Clear REG_INT 
    error = i2c_write_byte(keypad_data->i2c, REG_INT, 0x00);
    if(error) {
        dev_err(&keypad_data->i2c->dev, "unable to clear REG_INT\n");
    }

	return IRQ_HANDLED;
}
static int bbq10kbd_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id) 
{
  struct bbq10kbd_keypad *keypad_data;
  int ret, error;

  printk(KERN_DEBUG "bbq10kbd: probing");
  if(!i2c_check_functionality(client->adapter, 
        I2C_FUNC_SMBUS_BYTE_DATA | 
        I2C_FUNC_SMBUS_WORD_DATA))
  {
    printk(KERN_ERR "bbq10kbd: %s needed i2c functionality is not supported\n", __func__);
    return -ENODEV;
  }
  printk(KERN_DEBUG "bbq10kbd: configuring i2c device");
  keypad_data = devm_kzalloc(&client->dev, sizeof(struct bbq10kbd_keypad), GFP_KERNEL);
  if(keypad_data == NULL)
    return -ENOMEM; 
  keypad_data->i2c = client;

  // check fw version
  ret = i2c_read_byte(keypad_data->i2c, REG_VER);
  printk(KERN_DEBUG "bb10kbd: firmware version = %d", ret);

  //configure keyboard 
  ret = i2c_write_byte(keypad_data->i2c, REG_CFG, BBQ10_CFG_BITS);

  error = devm_gpio_request_one(&client->dev, INTERRUPT_GPIO, GPIOF_IN, "bbq10kbd_irq");
  if(error) {
      printk(KERN_ERR "bbq10kbd unable to claim interrupt gpio");
      return -ENODEV;
  }

  //setup irq handler
  printk(KERN_DEBUG "bbq10kbd: requesting irq handler %d", client->irq);
  error = devm_request_threaded_irq(&client->dev, client->irq, NULL, bbq10kbd_irq_handler,
        IRQF_SHARED | IRQF_ONESHOT,
        client->name, keypad_data);
  
  if (error) {
    dev_err(&client->dev, "Unable to claim irq %d; error %d\n",
    client->irq, error);
    return error;
  }

  
  ret = bbq10kbd_init_input_keyboard(keypad_data);
  if(ret != 0){
    printk(KERN_ERR "bbq10kbd: unable to initialise keyboard input device, returned %d\n", ret);
    return -ENODEV;
  }

  
  ret = bbq10kbd_init_input_pointer(keypad_data);
  if(ret != 0){
    printk(KERN_ERR "bbq10kbd: unable to initialise pointer input device, returned %d\n", ret);
    return -ENODEV;
  }

  
  // Device setup complete, set the client's device data
  i2c_set_clientdata(client, keypad_data);
  
  printk(KERN_DEBUG "bbq10kbd: probed.\n");
  return 0;
}

static int bbq10kbd_i2c_remove(struct i2c_client *client) {
  struct bbq10kbd_keypad *keypad_data = i2c_get_clientdata(client);
 
  printk(KERN_DEBUG "bbq10kbd: input_unregister_device");
  input_unregister_device(keypad_data->input_keyboard);
  input_unregister_device(keypad_data->input_pointer);
  printk(KERN_DEBUG "bbq10kbd: freeing device memory");
  kfree(keypad_data);

  printk(KERN_DEBUG "bbq10kbd: removed\n"); 
  return 0;  
}

static struct i2c_driver bbq10kbd_i2c_driver = {
  .driver = {
    .name = DEVICE_NAME,
    .owner = THIS_MODULE,
    .of_match_table = bbq10kbd_ids,
  },
  .probe = bbq10kbd_i2c_probe,
  .remove = bbq10kbd_i2c_remove,
  .id_table = bbq10kbd_id,
};

static int __init bbq10kbd_init(void) {
  int res;

  printk(KERN_DEBUG "bbq10kbd: initialising i2c...");
  res = i2c_add_driver(&bbq10kbd_i2c_driver);
  if(res != 0) {
    printk(KERN_DEBUG "bbq10kbd: i2c driver registration failed, module not inserted.\n");
    return res;
  }

  printk(KERN_DEBUG "bbq10kbd: inited\n");
  return res;
}

static void __exit bbq10kbd_exit(void) 
{
  printk(KERN_DEBUG "bbq10kbd: exiting...");
  printk(KERN_DEBUG "bbq10kbd: deleting i2c driver...");
  i2c_del_driver(&bbq10kbd_i2c_driver);

  printk("bbq10kbd: exited");
}

module_init(bbq10kbd_init);
module_exit(bbq10kbd_exit);
