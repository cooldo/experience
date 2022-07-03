//#nclude 
//#include 
//#include 
//#include 
//#include 
//#include 
//#include 

/* Playback rates supported by the codec */
static unsigned int mycard_rates[] = {
  8000,
  48000,
};
/* Hardware constraints for the playback channel */
static struct snd_pcm_hw_constraint_list mycard_playback_rates = {
  .count = ARRAY_SIZE(mycard_rates),
  .list = mycard_rates,
  .mask = 0,
};

static struct platform_device *mycard_device;
static char *id[SNDRV_CARDS] = SNDRV_DEFAULT_STR;

/* Hardware capabilities of the PCM stream */
static struct snd_pcm_hardware mycard_playback_stereo = {
  .info = (SNDRV_PCM_INFO_MMAP | SNDRV_PCM_INFO_BLOCK_TRANSFER),
  .formats = SNDRV_PCM_FMTBIT_S16_LE, /* 16 bits per channel, little endian */
  .rates = SNDRV_PCM_RATE_8000_48000, /* DAC Sampling rate range */
  .rate_min = 8000,                   /* Minimum sampling rate */
  .rate_max = 48000,                  /* Maximum sampling rate */
  .channels_min = 2,                  /* Supports a left and a right channel */
  .channels_max = 2,                  /* Supports a left and a right channel */
  .buffer_bytes_max = 32768,          /* Maximum buffer size */
};

/* Open the device in playback mode */
static int
mycard_pb_open(struct snd_pcm_substream *substream)
{
  struct snd_pcm_runtime *runtime = substream->runtime;

  /* Initialize driver structures */
  /* ... */
  /* Initialize codec registers */
  /* ... */
  /* Associate the hardware capabilities of this PCM component */
  runtime->hw = mycard_playback_stereo;

  /* Inform the ALSA framework about the constraints that
     the codec has. For example, in this case, it supports
     PCM sampling rates of 8000Hz and 48000Hz only */
  snd_pcm_hw_constraint_list(runtime, 0,
                             SNDRV_PCM_HW_PARAM_RATE,
                             &mycard_playback_rates);
  return 0;
}

/* Close */
static int
mycard_pb_close(struct snd_pcm_substream *substream)
{
  /* Disable the codec, stop DMA, free data structures */
  /* ... */
  return 0;
}

/* Write to codec registers by communicating over
   the bus that connects the SoC to the codec */
void
codec_write_reg(uint codec_register, uint value)
{
  /* ... */
}
/* Prepare to transfer an audio stream to the codec */
static int
mycard_pb_prepare(struct snd_pcm_substream *substream)
{

  /* Enable Transmit DMA complete interrupt by writing to
     CONTROL_REGISTER using codec_write_reg() */

  /* Set the sampling rate by writing to SAMPLING_RATE_REGISTER */

  /* Configure clock source and enable clocking by writing
     to CLOCK_INPUT_REGISTER */

  /* Allocate DMA descriptors for audio transfer */

  return 0;
}

/* Audio trigger/stop/.. */
static int
mycard_pb_trigger(struct snd_pcm_substream *substream, int cmd)
{
  switch (cmd) {
  case SNDRV_PCM_TRIGGER_START:
    /* Map the audio substream's runtime audio buffer (which is an
       offset into runtime->dma_area) using dma_map_single(),
       populate the resulting address to the audio controller's
       DMA_ADDRESS_REGISTER, and perform DMA */
    /* ... */
    break;

  case SNDRV_PCM_TRIGGER_STOP:
    /* Shut the stream. Unmap DMA buffer using dma_unmap_single() */
    /* ... */
    break;

  default:
    return -EINVAL;
    break;
  }

  return 0;
}
/* Allocate DMA buffers using memory preallocated for DMA from the
   probe() method. dma_[map|unmap]_single() operate on this area
   later on */
static int
mycard_hw_params(struct snd_pcm_substream *substream,
                 struct snd_pcm_hw_params *hw_params)
{
  /* Use preallocated memory from mycard_audio_probe() to
     satisfy this memory request */
  return snd_pcm_lib_malloc_pages(substream,
                                  params_buffer_bytes(hw_params));
}

/* Reverse of mycard_hw_params() */
static int
mycard_hw_free(struct snd_pcm_substream *substream)
{
  return snd_pcm_lib_free_pages(substream);
}

/* Volume info */
static int
mycard_pb_vol_info(struct snd_kcontrol *kcontrol,
                   struct snd_ctl_elem_info *uinfo)
{
  uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
                                  /* Integer type */
  uinfo->count = 1;               /* Number of values */
  uinfo->value.integer.min = 0;   /* Minimum volume gain */
  uinfo->value.integer.max = 10;  /* Maximum volume gain */
  uinfo->value.integer.step = 1;  /* In steps of 1 */
  return 0;
}

/* Playback volume knob */
static int
mycard_pb_vol_put(struct snd_kcontrol *kcontrol,
                  struct snd_ctl_elem_value *uvalue)
{
  int global_volume = uvalue->value.integer.value[0];

  /* Write global_volume to VOLUME_REGISTER
     using codec_write_reg() */
  /* ... */
  /* If the volume changed from the current value, return 1.
     If there is an error, return negative code. Else return 0 */
}

/* Get playback volume */
static int
mycard_pb_vol_get(struct snd_kcontrol *kcontrol,
                  struct snd_ctl_elem_value *uvalue)
{
  /* Read global_volume from VOLUME_REGISTER
     and return it via uvalue->integer.value[0] */
  /* ... */
  return 0;
}

/* Entry points for the playback mixer */
static struct snd_kcontrol_new mycard_playback_vol = {
  .iface = SNDRV_CTL_ELEM_IFACE_MIXER,
                                /* Control is of type MIXER */
  .name  = "MP3 Volume",        /* Name */
  .index = 0,                   /* Codec No: 0 */
  .info  = mycard_pb_vol_info,  /* Volume info */
  .get   = mycard_pb_vol_get,   /* Get volume */
  .put   = mycard_pb_vol_put,   /* Set volume */
};

/* Operators for the PCM playback stream */
static struct snd_pcm_ops mycard_playback_ops = {
  .open      = mycard_playback_open,    /* Open */
  .close     = mycard_playback_close,   /* Close */
  .ioctl     = snd_pcm_lib_ioctl,       /* Generic ioctl handler */
  .hw_params = mycard_hw_params,        /* Hardware parameters */
  .hw_free   = mycard_hw_free,          /* Free h/w params */
  .prepare   = mycard_playback_prepare, /* Prepare to transfer audio stream */
  .trigger   = mycard_playback_trigger, /* Called when the PCM engine
                                           starts/stops/pauses */
};

/* Platform driver probe() method */
static int __init
mycard_audio_probe(struct platform_device *dev)
{
  struct snd_card *card;
  struct snd_pcm *pcm;
  int myctl_private;

  /* Instantiate an snd_card structure */
  card = snd_card_new(-1, id[dev->id], THIS_MODULE, 0);

  /* Create a new PCM instance with 1 playback substream
     and 0 capture streams */
  snd_pcm_new(card, "mycard_pcm", 0, 1, 0, &pcm);

  /* Set up our initial DMA buffers */
  snd_pcm_lib_preallocate_pages_for_all(pcm,
                         SNDRV_DMA_TYPE_CONTINUOUS,
                         snd_dma_continuous_data
                         (GFP_KERNEL), 256*1024,
                         256*1024);

  /* Connect playback operations with the PCM instance */
  snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_PLAYBACK,
                  &mycard_playback_ops);

  /* Associate a mixer control element with this card */
  snd_ctl_add(card, snd_ctl_new1(&mycard_playback_vol,
                                 &myctl_private));

  strcpy(card->driver, "mycard");

  /* Register the sound card */
  snd_card_register(card);

  /* Store card for access from other methods */
  platform_set_drvdata(dev, card);

  return 0;
}

/* Platform driver remove() method */
static int
mycard_audio_remove(struct platform_device *dev)
{
  snd_card_free(platform_get_drvdata(dev));
  platform_set_drvdata(dev, NULL);
  return 0;
}

/* Platform driver definition */
static struct platform_driver mycard_audio_driver = {
  .probe = mycard_audio_probe,     /* Probe method */
  .remove = mycard_audio_remove,   /* Remove method */
  .driver = {
    .name = "mycard_ALSA",
  },
};

/* Driver Initialization */
static int __init
mycard_audio_init(void)
{
  /* Register the platform driver and device */
  platform_driver_register(&mycard_audio_driver);

  mycard_device = platform_device_register_simple("mycard_ALSA",
                                                  -1, NULL, 0);
  return 0;
}

/* Driver Exit */
static void __exit
mycard_audio_exit(void)
{
  platform_device_unregister(mycard_device);
  platform_driver_unregister(&mycard_audio_driver);
}

module_init(mycard_audio_init);
module_exit(mycard_audio_exit);
MODULE_LICENSE("GPL");
