#include "mbed.h"
#include <cmath>
#include "DA7212.h"
#include "uLCD_4DGL.h"

#include "accelerometer_handler.h"

#include "config.h"

#include "magic_wand_model_data.h"


#include "tensorflow/lite/c/common.h"

#include "tensorflow/lite/micro/kernels/micro_ops.h"

#include "tensorflow/lite/micro/micro_error_reporter.h"

#include "tensorflow/lite/micro/micro_interpreter.h"

#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"

#include "tensorflow/lite/schema/schema_generated.h"

#include "tensorflow/lite/version.h"


DA7212 audio;
uLCD_4DGL uLCD(D1, D0, D2);
int16_t waveform[kAudioTxBufferSize];
Serial pc(USBTX, USBRX);
InterruptIn sw2(SW2);
DigitalIn sw3(SW3);
EventQueue queue1(32 * EVENTS_EVENT_SIZE);
EventQueue queue2(32 * EVENTS_EVENT_SIZE);
Thread t1;
Thread t2;



constexpr int kTensorArenaSize = 60 * 1024;
uint8_t tensor_arena[kTensorArenaSize];
bool should_clear_buffer = false;
bool got_data = false;



int song0[32] = {
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1};
int song1[32] = {
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1};
int song2[32] = {
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1};
int noteLength[32] = {
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1};
int loadflag=0;
int gesture=-1;
int pulse=0;
int mode=0;
int songid=1;
int selectflag=0;

void playNote(int freq)
{
  for(int i = 0; i < kAudioTxBufferSize; i++)
  {
    waveform[i] = (int16_t) (sin((double)i * 2. * M_PI/(double) (kAudioSampleFrequency / freq)) * ((1<<16) - 1));
  }
  audio.spk.play(waveform, kAudioTxBufferSize);
}

void song_thread()
{

 

  if(loadflag==0){
    char serialInBuffer[32];
    int serialCount = 0;
    int i = 0;
    while(i < 32)
    {
      if(pc.readable())
      {
        serialInBuffer[serialCount] = pc.getc();
        //pc.printf("%d,%d,%c     ", i, serialCount, serialInBuffer[serialCount]);
        serialCount++;
        if(serialCount == 3)
        {
          serialInBuffer[serialCount] = '\0';
          song0[i] = (int) atof(serialInBuffer);
          //pc.printf("%d,%d     ", i, song[i]);
          serialCount = 0;
          i++;
        }
      }
    }
    i = 0;
    while(i < 32)
    {
      if(pc.readable())
      {
        serialInBuffer[serialCount] = pc.getc();
        //pc.printf("%d,%d,%c     ", i, serialCount, serialInBuffer[serialCount]);
        serialCount++;
        if(serialCount == 3)
        {
          serialInBuffer[serialCount] = '\0';
          song1[i] = (int) atof(serialInBuffer);
          //pc.printf("%d,%d     ", i, song[i]);
          serialCount = 0;
          i++;
        }
      }
    }
    i = 0;
    while(i < 32)
    {
      if(pc.readable())
      {
        serialInBuffer[serialCount] = pc.getc();
        //pc.printf("%d,%d,%c     ", i, serialCount, serialInBuffer[serialCount]);
        serialCount++;
        if(serialCount == 3)
        {
          serialInBuffer[serialCount] = '\0';
          song2[i] = (int) atof(serialInBuffer);
          //pc.printf("%d,%d     ", i, song[i]);
          serialCount = 0;
          i++;
        }
      }
    }
    loadflag=1;
  }
  
  if(mode==2){
    selectflag++;
    if(selectflag>=2){
      selectflag=0;
    }else{
      uLCD.cls();
      uLCD.printf("song %d\n", songid);
      pc.printf("enter song selection mode...\n\r");
    }
  }
  if(selectflag==0){
    if(mode==0){
      songid--;
    }else if(mode==1){
      songid++;
    }
    if(songid==-1){
      songid=2;
    }else if(songid==3){
      songid=0;
    }
    
    if(songid==0){
      uLCD.cls();
      uLCD.printf("playing song %d\n", songid);
      for(int i = 0; i < 32; i++)
      {
        int length = noteLength[i];
        while(length--)
        {
          // the loop below will play the note for the duration of 1s
          for(int j = 0; j < kAudioSampleFrequency / kAudioTxBufferSize; ++j)
          {
            //queue2.call(playNote, song0[i]);
            playNote(song0[i]);
          }
          if(length < 1) wait(0.01);
          
        }
        if(sw3==0){
          break;
        }
      }
    }
    if(songid==1){
      uLCD.cls();
      uLCD.printf("playing song %d\n", songid);
      for(int i = 0; i < 32; i++)
      {
        int length = noteLength[i];
        while(length--)
        {
          // the loop below will play the note for the duration of 1s
          for(int j = 0; j < kAudioSampleFrequency / kAudioTxBufferSize; ++j)
          {
            //queue2.call(playNote, song1[i]);
            playNote(song1[i]);
          }
          if(length < 1) wait(0.01);
          
        }
        if(sw3==0){
          break;
        }
      }
    }
    if(songid==2){
      uLCD.cls();
      uLCD.printf("playing song %d\n", songid);
      for(int i = 0; i < 32; i++)
      {
        int length = noteLength[i];
        while(length--)
        {
          // the loop below will play the note for the duration of 1s
          for(int j = 0; j < kAudioSampleFrequency / kAudioTxBufferSize; ++j)
          {
            //queue2.call(playNote, song2[i]);
            playNote(song2[i]);
          }
          if(length < 1) wait(0.01);
          
        }
        if(sw3==0){
            break;
        }
      }
    }
    audio.spk.pause();

  }
}

void mode_thread(){
  while(true){
    if(pulse==1){
      pc.printf("%d!!!!!!     ", gesture);
      if(gesture==0){
        mode++;
      }else{
        mode--;
      }
      if(mode==-1){
        mode=2;
      }else if(mode==3){
        mode=0;
      }  
      pulse=0;
    }
    pc.printf("%d   ", mode);
  }
}



// Return the result of the last prediction

int PredictGesture(float* output) {
  // How many times the most recent gesture has been matched in a row
  static int continuous_count = 0;
  // The result of the last prediction
  static int last_predict = -1;
  // Find whichever output has a probability > 0.8 (they sum to 1)
  int this_predict = -1;
  for (int i = 0; i < label_num; i++) {
    if (output[i] > 0.8) this_predict = i;
  }
  // No gesture was detected above the threshold
  if (this_predict == -1) {
    continuous_count = 0;
    last_predict = label_num;
    return label_num;
  }

  if (last_predict == this_predict) {
    continuous_count += 1;
  } else {
    continuous_count = 0;
  }

  last_predict = this_predict;
  // If we haven't yet had enough consecutive matches for this gesture,
  // report a negative result
  if (continuous_count < config.consecutiveInferenceThresholds[this_predict]) {
    return label_num;
  }
  // Otherwise, we've seen a positive result, so clear all our variables
  // and report it
  continuous_count = 0;
  last_predict = -1;
  return this_predict;
}


int main(int argc, char* argv[]) {


  // Create an area of memory to use for input, output, and intermediate arrays.

  // The size of this will depend on the model you're using, and may need to be

  // determined by experimentation.

  //constexpr int kTensorArenaSize = 60 * 1024;

  //uint8_t tensor_arena[kTensorArenaSize];


  // Whether we should clear the buffer next time we fetch data

  //bool should_clear_buffer = false;

  //bool got_data = false;


  // The gesture index of the prediction

  int gesture_index;


  // Set up logging.

  static tflite::MicroErrorReporter micro_error_reporter;

  tflite::ErrorReporter* error_reporter = &micro_error_reporter;


  // Map the model into a usable data structure. This doesn't involve any

  // copying or parsing, it's a very lightweight operation.

  const tflite::Model* model = tflite::GetModel(g_magic_wand_model_data);

  if (model->version() != TFLITE_SCHEMA_VERSION) {

    error_reporter->Report(

        "Model provided is schema version %d not equal "

        "to supported version %d.",

        model->version(), TFLITE_SCHEMA_VERSION);

    return -1;

  }


  // Pull in only the operation implementations we need.

  // This relies on a complete list of all the ops needed by this graph.

  // An easier approach is to just use the AllOpsResolver, but this will

  // incur some penalty in code space for op implementations that are not

  // needed by this graph.

  static tflite::MicroOpResolver<6> micro_op_resolver;

  micro_op_resolver.AddBuiltin(

      tflite::BuiltinOperator_DEPTHWISE_CONV_2D,

      tflite::ops::micro::Register_DEPTHWISE_CONV_2D());

  micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_MAX_POOL_2D,

                               tflite::ops::micro::Register_MAX_POOL_2D());

  micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_CONV_2D,

                               tflite::ops::micro::Register_CONV_2D());

  micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_FULLY_CONNECTED,

                               tflite::ops::micro::Register_FULLY_CONNECTED());

  micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_SOFTMAX,

                               tflite::ops::micro::Register_SOFTMAX());
micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_RESHAPE,
                             tflite::ops::micro::Register_RESHAPE(), 1);

  // Build an interpreter to run the model with

  static tflite::MicroInterpreter static_interpreter(

      model, micro_op_resolver, tensor_arena, kTensorArenaSize, error_reporter);

  tflite::MicroInterpreter* interpreter = &static_interpreter;


  // Allocate memory from the tensor_arena for the model's tensors

  interpreter->AllocateTensors();


  // Obtain pointer to the model's input tensor

  TfLiteTensor* model_input = interpreter->input(0);

  if ((model_input->dims->size != 4) || (model_input->dims->data[0] != 1) ||

      (model_input->dims->data[1] != config.seq_length) ||

      (model_input->dims->data[2] != kChannelNumber) ||

      (model_input->type != kTfLiteFloat32)) {

    error_reporter->Report("Bad input tensor parameters in model");

    return -1;

  }


  int input_length = model_input->bytes / sizeof(float);


  TfLiteStatus setup_status = SetupAccelerometer(error_reporter);

  if (setup_status != kTfLiteOk) {

    error_reporter->Report("Set up failed\n");

    return -1;

  }


  error_reporter->Report("Set up successful...\n");



  t1.start(callback(&queue1, &EventQueue::dispatch_forever));
  t2.start(callback(&queue2, &EventQueue::dispatch_forever));
  sw2.fall(queue1.event(song_thread));
  

  while (true) {

    // Attempt to read new data from the accelerometer

    got_data = ReadAccelerometer(error_reporter, model_input->data.f,

                                 input_length, should_clear_buffer);


    // If there was no new data,

    // don't try to clear the buffer again and wait until next time

    if (!got_data) {

      should_clear_buffer = false;

      continue;

    }


    // Run inference, and report any error

    TfLiteStatus invoke_status = interpreter->Invoke();

    if (invoke_status != kTfLiteOk) {

      error_reporter->Report("Invoke failed on index: %d\n", begin_index);

      continue;

    }


    // Analyze the results to obtain a prediction

    gesture_index = PredictGesture(interpreter->output(0)->data.f);


    // Clear the buffer next time we read data

    should_clear_buffer = gesture_index < label_num;


    // Produce an output

    if (gesture_index < label_num) {

      error_reporter->Report(config.output_message[gesture_index]);
      gesture=gesture_index;
      pc.printf("gesture %d    ", gesture);
      if(selectflag==0){
        if(gesture==0){
          mode++;
        }else{
          mode--;
        }
        if(mode==-1){
          mode=2;
        }else if(mode==3){
          mode=0;
        }
        uLCD.cls();
        if(mode==0){
          uLCD.printf("\nbackward\n");
        }else if(mode==1){
          uLCD.printf("\nforward\n");
        }else if(mode==2){
          uLCD.printf("\nsong select\n");
        }
        pc.printf("mode %d\n\r", mode);
      }
      if(selectflag==1){
        if(gesture==0){
          songid++;
        }else{
          songid--;
        }
        if(songid==-1){
          songid=2;
        }else if(songid==3){
          songid=0;   
        }
        uLCD.cls();
        uLCD.printf("\nsong %d\n",songid);
        pc.printf("songid %d\n\r", songid);
      }
    }
  }
}