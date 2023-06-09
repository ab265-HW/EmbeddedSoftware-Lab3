
#define Output_Pin_1 13 //Actual Pin Selected
#define Output_Pin_2 14 //Actual Pin Selected
#define Input_Pin_1 0 //Actual Pin Selected
#define Input_Pin_2 2 //Actual Pin Selected
#define Input_Pin_3 4 //Actual Pin Selected

#define PWM_Out_Pin 15 //Actual Pin Selected


int const reqSamples = 4;
int readSamplesCount = 0;
int readSamples[reqSamples];

struct FreqTaskDesc {
  int minFreq;
  int maxFreq;
  float measuredFreq;
};

struct FreqResults {
  struct FreqTaskDesc task2Desc;
  struct FreqTaskDesc task3Desc;
};
struct FreqResults *globalFreqResultsPtr, globalFreqResults;

void setup() {
  Serial.begin(9600);
  pinMode(PWM_Out_Pin, OUTPUT);
  ledcSetup(0, 500, 8);
  ledcAttachPin(PWM_Out_Pin, 0);
  ledcWrite(0, 127);

  
  globalFreqResultsPtr = &globalFreqResults;
  
  globalFreqResults.task2Desc.minFreq = 333;
  globalFreqResults.task2Desc.maxFreq = 1000;

  globalFreqResults.task3Desc.minFreq = 500;
  globalFreqResults.task3Desc.maxFreq = 1000;

   xTaskCreate(task2,   // pointer to the task function
              "Task 2", // task name
              1000,      // stack size in words
              (void*)globalFreqResultsPtr,     // stack parameter
              1,        // priority
              NULL);    // task handle (not used)
              
   xTaskCreate(task3,   // pointer to the task function
              "Task 3", // task name
              1000,      // stack size in words
              (void*)globalFreqResultsPtr,     // stack parameter
              1,        // priority
              NULL);    // task handle (not used)

}

void loop() {
  // put your main code here, to run repeatedly:
}

void task1(void *Ptr){
  
  TickType_t lastWake = xTaskGetTickCount();
  const TickType_t period = 500/portTICK_PERIOD_MS;

  for( ;; )
    {
      digitalWrite(Output_Pin_1, HIGH);
      delayMicroseconds(200);
      digitalWrite(Output_Pin_1,LOW);
      delayMicroseconds(50);
      digitalWrite(Output_Pin_1,HIGH);
      delayMicroseconds(30);
      digitalWrite(Output_Pin_1,LOW);
      vTaskDelayUntil(&lastWake,period);
    }
}

void task2(void *freqResultsPtr){

  TickType_t lastWake = xTaskGetTickCount();
  const TickType_t period = 500/portTICK_PERIOD_MS;

  for( ;; )
    {
      struct FreqResults *freqResPtr = (struct FreqResults *) freqResultsPtr;
      //This is the line needing edited to add new tasks
      struct FreqTaskDesc *taskDescPtr = &(freqResPtr->task2Desc);
      
      int waitTime = (int)((1000000.0/(taskDescPtr->minFreq))+10);
      
      bool currentState = digitalRead(0);
      int halfWaveLengthMicro = pulseIn(0, !currentState, waitTime);
      if (halfWaveLengthMicro <= 0) {(taskDescPtr->measuredFreq) = 0;}
      else {int WaveLengthMicro = 2*halfWaveLengthMicro;(taskDescPtr->measuredFreq) = 1000000/WaveLengthMicro;}
      Serial.println("Task2 Frequency is: " + String(globalFreqResults.task2Desc.measuredFreq));
      vTaskDelayUntil(&lastWake,period);
    }
}

void task3(void *freqResultsPtr){

  TickType_t lastWake = xTaskGetTickCount();
  const TickType_t period = 500/portTICK_PERIOD_MS;

  for( ;; )
    {
      struct FreqResults *freqResPtr = (struct FreqResults *) freqResultsPtr;
      //This is the line needing edited to add new tasks
      struct FreqTaskDesc *taskDescPtr = &(freqResPtr->task3Desc);
      
      int waitTime = (int)((1000000.0/(taskDescPtr->minFreq))+10);
      
      bool currentState = digitalRead(0);
      int halfWaveLengthMicro = pulseIn(0, !currentState, waitTime);
      if (halfWaveLengthMicro <= 0) {(taskDescPtr->measuredFreq) = 0;}
      else {int WaveLengthMicro = 2*halfWaveLengthMicro;(taskDescPtr->measuredFreq) = 1000000/WaveLengthMicro;}
      Serial.println("Task3 Frequency is: " + String(globalFreqResults.task3Desc.measuredFreq));
      vTaskDelayUntil(&lastWake,period);
    }
}

void task4(void *Ptr) {

  TickType_t lastWake = xTaskGetTickCount();
  const TickType_t period = 500/portTICK_PERIOD_MS;

    for( ;; )
    {
      if( readSamplesCount == 2*reqSamples) { readSamplesCount == reqSamples; }
    
      int i = readSamplesCount % reqSamples;
      readSamples[i]= analogRead(Input_Pin_1);
      readSamplesCount++;
    
      //Averaging Code
      int averageValue = 0;
      //this computes the Average before enough readings have been taken to fill the cyclic buffer
      if (readSamplesCount < reqSamples) {
        for (int i = 0; i< readSamplesCount; i++) { averageValue += readSamples[i]; }
        averageValue = averageValue/readSamplesCount;
      }
      //this computes the average when the cyclic buffer has been filled
      else {
        for (int i = 0; i<reqSamples; i++) { averageValue += readSamples[i]; }
        averageValue = averageValue/reqSamples;
      }
    
      //if the average value is half of the maximum allowed analog read value, then output a high signal on the Output pin otherwise, output a Low signal
      if(averageValue > 2047) { digitalWrite(Output_Pin_2,HIGH);}
      else{ digitalWrite(Output_Pin_2,LOW);}
      vTaskDelayUntil(&lastWake,period);
    }
}
