
/*******************************************************************************
** Toolset-ts-global-app                                                      **
** MIT License                                                                **
** Copyright (c) [2018] [Florian Lance]                                       **
**                                                                            **
** Permission is hereby granted, free of charge, to any person obtaining a    **
** copy of this software and associated documentation files (the "Software"), **
** to deal in the Software without restriction, including without limitation  **
** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
** and/or sell copies of the Software, and to permit persons to whom the      **
** Software is furnished to do so, subject to the following conditions:       **
**                                                                            **
** The above copyright notice and this permission notice shall be included in **
** all copies or substantial portions of the Software.                        **
**                                                                            **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
** DEALINGS IN THE SOFTWARE.                                                  **
**                                                                            **
********************************************************************************/

#include "tests.hpp"


// std
#include <iostream>

// soundio
#include "soundio.h"


static auto read_callback(struct SoundIoInStream *instream, int frame_count_min, int frame_count_max) -> void {

    // struct SoundIoChannelArea *areas;
    // int err;
    // char *write_ptr = soundio_ring_buffer_write_ptr(ring_buffer);
    // int free_bytes = soundio_ring_buffer_free_count(ring_buffer);
    // int free_count = free_bytes / instream->bytes_per_frame;

    // if (frame_count_min > free_count)
    //     panic("ring buffer overflow");

    // int write_frames = min_int(free_count, frame_count_max);
    // int frames_left = write_frames;

    // for (;;) {
    //     int frame_count = frames_left;

    //     if ((err = soundio_instream_begin_read(instream, &areas, &frame_count)))
    //         panic("begin read error: %s", soundio_strerror(err));

    //     if (!frame_count)
    //         break;

    //     if (!areas) {
    //         // Due to an overflow there is a hole. Fill the ring buffer with
    //         // silence for the size of the hole.
    //         memset(write_ptr, 0, frame_count * instream->bytes_per_frame);
    //         fprintf(stderr, "Dropped %d frames due to internal overflow\n", frame_count);
    //     } else {
    //         for (int frame = 0; frame < frame_count; frame += 1) {
    //             for (int ch = 0; ch < instream->layout.channel_count; ch += 1) {
    //                 memcpy(write_ptr, areas[ch].ptr, instream->bytes_per_sample);
    //                 areas[ch].ptr += areas[ch].step;
    //                 write_ptr += instream->bytes_per_sample;
    //             }
    //         }
    //     }

    //     if ((err = soundio_instream_end_read(instream)))
    //         panic("end read error: %s", soundio_strerror(err));

    //     frames_left -= frame_count;
    //     if (frames_left <= 0)
    //         break;
    // }

    // int advance_bytes = write_frames * instream->bytes_per_frame;
    // soundio_ring_buffer_advance_write_ptr(ring_buffer, advance_bytes);
}


auto microphone() -> void{

    std::cout << "test_microphone\n";

    enum SoundIoBackend backend = SoundIoBackendWasapi;
    std::cout << "soundio_create\n";
    auto soundio = soundio_create();
    if(!soundio){
        std::cerr << "out of memory\n";
        return;
    }

    std::cout << "soundio_connect\n";
    int err = 0;
    if(backend == SoundIoBackendNone){
        err = soundio_connect(soundio);
    }else{
        err = soundio_connect_backend(soundio, backend);
    }
    std::cout << "-->" << err << "\n";

    bool connected = false;
    if(err == 0){
        connected = true;
    }else{
        std::cerr << std::format("error connecting {}\n", soundio_strerror(err));
    }

    if(connected){
        soundio_flush_events(soundio);
    }

    int defaultOuputDeviceId = soundio_default_output_device_index(soundio);
    if (defaultOuputDeviceId < 0){
        std::cerr << "no output device found\n";
    }

    int defaultInputdeviceiD = soundio_default_input_device_index(soundio);
    if (defaultInputdeviceiD < 0){
        std::cerr << "no input device found\n";
    }
    std::cout << "default output device index " << defaultOuputDeviceId << " input device " << defaultInputdeviceiD << "\n";

    int inputDevicesCount = soundio_input_device_count(soundio);
    int outputDevicesCount = soundio_output_device_count(soundio);
    std::cout << "inputDevicesCount " << inputDevicesCount << " outputDevicesCount " << outputDevicesCount << "\n";


    std::string wantedDevice = "Microphone (HD Webcam C525)";
    bool useRawDevice = true;

    bool found = false;
    int correpondingInputDeviceId = 0;
    if(defaultInputdeviceiD >= 0){
        for (int id = 0; id < soundio_input_device_count(soundio); ++id) {
            auto device = soundio_get_input_device(soundio, id);

            std::cout << id << " | "  <<  device->id << " [" << device->name << "]\n";
            if(useRawDevice == device->is_raw){
                if(wantedDevice == std::string(device->name)){
                    correpondingInputDeviceId = id;
                    found = true;
                    soundio_device_unref(device);
                    break;
                }
            }

            soundio_device_unref(device);
        }
    }

    std::cout << "device found " << found << " with id " << correpondingInputDeviceId << "\n";
    if(found){
        auto inDevice = soundio_get_input_device(soundio, correpondingInputDeviceId);

        // const struct SoundIoChannelLayout *layout = soundio_best_matching_channel_layout(
        //     nullptr, 0,
        //     inDevice->layouts, inDevice->layout_count
        // );

        std::cout << "layout count " << inDevice->layout_count << "\n";
        for(int idL = 0; idL < inDevice->layout_count; ++idL){
            const auto &layout = inDevice->layouts[idL];
            std::cout << "channels count " << layout.channel_count << "\n";
            std::cout << "name" << layout.name << "\n";
        }



        std::cout << "support format SoundIoFormatInvalid " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatInvalid) << "\n";
        std::cout << "support format SoundIoFormatS8 " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatS8) << "\n";
        std::cout << "support format SoundIoFormatU8 " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatU8) << "\n";
        std::cout << "support format SoundIoFormatS16LE " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatS16LE) << "\n";
        std::cout << "support format SoundIoFormatS16BE " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatS16BE) << "\n";
        std::cout << "support format SoundIoFormatU16LE " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatU16LE) << "\n";
        std::cout << "support format SoundIoFormatU16BE " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatU16BE) << "\n";
        std::cout << "support format SoundIoFormatS24LE " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatS24LE) << "\n";
        std::cout << "support format SoundIoFormatS24BE " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatS24BE) << "\n";
        std::cout << "support format SoundIoFormatU24LE " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatU24LE) << "\n";
        std::cout << "support format SoundIoFormatU24BE " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatU24BE) << "\n";
        std::cout << "support format SoundIoFormatS32LE " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatS32LE) << "\n";
        std::cout << "support format SoundIoFormatS32BE " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatS32BE) << "\n";
        std::cout << "support format SoundIoFormatU32LE " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatU32LE) << "\n";
        std::cout << "support format SoundIoFormatU32BE " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatU32BE) << "\n";
        std::cout << "support format SoundIoFormatFloat32LE " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatFloat32LE) << "\n";
        std::cout << "support format SoundIoFormatFloat32BE " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatFloat32BE) << "\n";
        std::cout << "support format SoundIoFormatFloat64LE " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatFloat64LE) << "\n";
        std::cout << "support format SoundIoFormatFloat64BE " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatFloat64BE) << "\n";

        std::cout << "support samplerate 48000 " << soundio_device_supports_sample_rate(inDevice, 48000) << "\n";
        std::cout << "support samplerate 44100 " << soundio_device_supports_sample_rate(inDevice, 44100) << "\n";
        std::cout << "support samplerate 96000 " << soundio_device_supports_sample_rate(inDevice, 96000) << "\n";
        std::cout << "support samplerate 24000 " << soundio_device_supports_sample_rate(inDevice, 24000) << "\n";
        std::cout << "support samplerate 0 " << soundio_device_supports_sample_rate(inDevice, 0) << "\n";
        // soundio_device_nearest_sample_rate()

        auto instream = soundio_instream_create(inDevice);
        if(!instream){
            std::cerr << "out of memory\n";
        }

        double microphone_latency = 0.05;
        instream->format = SoundIoFormat::SoundIoFormatS16LE;
        instream->sample_rate = 48000;
        instream->layout = inDevice->layouts[0];
        instream->software_latency = microphone_latency;
        instream->read_callback = read_callback;

        if ((err = soundio_instream_open(instream))) {
            std::cerr << "unable to open output stream: " << soundio_strerror(err) << "\n";
        }

        int capacity = microphone_latency * 2 * instream->sample_rate * instream->bytes_per_frame;
        auto ring_buffer = soundio_ring_buffer_create(soundio, capacity);


        // soundio_wait_events(soundio);


        soundio_ring_buffer_destroy(ring_buffer);
        soundio_instream_destroy(instream);
        soundio_device_unref(inDevice);
    }
    // ...

    std::cout << "soundio_destroy\n";
    soundio_destroy(soundio);

    std::cout << "end test_microphone\n";
}
