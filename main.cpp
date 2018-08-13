/*
 * main.cpp
 *
 *  Created on: Aug 10, 2018
 *      Author: vnbk
 */
#include <iostream>      // standard library
#include <string.h>
#include <thread>
#include <stdlib.h>

#include "picovoice.h"
#include "pv_porcupine.h"
#include <portaudio.h>   // for record audio

pv_porcupine_object_t *handle;

int send_audio(const void *input,
             void *output,
             unsigned long frameCount,
             const PaStreamCallbackTimeInfo* timeInfo,
             PaStreamCallbackFlags statusFlags,
             void *userData){
	bool result;

	/**/

	const int16_t* pcm = (const int16_t*)input;
	const pv_status_t status = pv_porcupine_process(handle, pcm, &result);
	if (status != PV_STATUS_SUCCESS) {
		// error handling logic
		std::cout << "Error porcupine process" << std::endl;
	}
	if (result) {
		// detection event logic/callback
		std::cout << "Detected Hotword!!!" << std::endl;
	}
	return 0;
}

int main(int argc, char** argv){
	const char *model_file_path = argv[1]; // "/media/vnbk/Data/Projects/workspace/TestHotwordPicovoice/Picovoice/libPicovoice/porcupine_params.pv"; // The file is available at lib/common/porcupine_params.pv
	const char *keyword_file_path = argv[2]; // "/media/vnbk/Data/Projects/workspace/TestHotwordPicovoice/Picovoice/libPicovoice/vancouver_linux.ppn";
	const float sensitivity = atof(argv[3]);
	const unsigned long frameFerBuffer = atoi(argv[4]);


	const pv_status_t status = pv_porcupine_init(model_file_path, keyword_file_path, sensitivity, &handle);

	if (status != PV_STATUS_SUCCESS) {
	    // error handling logic
		std::cout << "Error Initial Porcupine" << std::endl;
		return -1;
	}

	const int porcupineFrameLength = pv_porcupine_frame_length();
	std::cout << "Frame length porcupine " << porcupineFrameLength << std::endl;

    PaStream *stream;
    Pa_Initialize();

    Pa_OpenDefaultStream(&stream,
                         1,          /* no input channels */
                         0,          /* stereo output */
                         paInt16,  /* 32 bit floating point output */
                         16000,
						 frameFerBuffer,        /* frames per buffer */
                         send_audio, /* this is your callback function */
                         NULL);     /*This is a pointer that will be passed to your callback*/

    Pa_StartStream(stream);

    while(1)
        Pa_Sleep(10000);
    pv_porcupine_delete(handle);
    return 0;
}
