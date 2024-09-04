

/***********************************************************************************
** MIT License                                                                    **
** Copyright (c) [2024] [Florian Lance]                                           **
** Permission is hereby granted, free of charge, to any person obtaining a copy   **
** of this software and associated documentation files (the "Software"), to deal  **
** in the Software without restriction, including without limitation the rights   **
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      **
** copies of the Software, and to permit persons to whom the Software is          **
** furnished to do so, subject to the following conditions:                       **
**                                                                                **
** The above copyright notice and this permission notice shall be included in all **
** copies or substantial portions of the Software.                                **
**                                                                                **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    **
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  **
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  **
** SOFTWARE.                                                                      **
************************************************************************************/

// system
using System.Threading;

// unity
using UnityEngine.Profiling;


namespace BS {
    public class DCReadNetworkDataJob : ThreadedJob {

        public DCClientDLL dllPlayer = null;
        public int idD = 0;
        public volatile bool doLoop = false;

        public void start_reading() {
            doLoop = true;
            start();
        }

        public void stop_reading() {
            doLoop = false;
            stop();
        }

        protected override void thread_function() {

            int totalPackedRead = 0;
            Thread.CurrentThread.Name = string.Concat("ReadNetworkData ", idD);
            Profiler.BeginThreadProfiling("ReadNetworkData", Thread.CurrentThread.Name);
            while (doLoop) {
                Profiler.BeginSample("[DCNetworkDirectPlayer::read_network_data]");                
                int nbPackedRead = dllPlayer.read_network_data(idD);
                Profiler.EndSample();
                totalPackedRead += nbPackedRead;
            };
            Profiler.EndThreadProfiling();
        }
    }

    public class DCTriggerReceivedPacketsJob : ThreadedJob {

        public DCClientDLL dllPlayer = null;
        public int idD = 0;
        public volatile bool doLoop = false;

        public void start_triggering() {
            doLoop = true;
            start();
        }

        public void stop_triggering() {
            doLoop = false;
            stop();
        }

        protected override void thread_function() {

            Thread.CurrentThread.Name = string.Concat("TriggerReceivedPackets ", idD);
            Profiler.BeginThreadProfiling("TriggerReceivedPackets", Thread.CurrentThread.Name);
            while (doLoop) {
                Profiler.BeginSample("[DCNetworkDirectPlayer::trigger_received_packets]");
                dllPlayer.trigger_received_packets(idD);
                Profiler.EndSample();
            }
            Profiler.EndThreadProfiling();
        }
    }

    public class DCProcessDataJob : ThreadedJob {

        public DCClientDLL dllPlayer = null;
        public int idD = 0;
        public volatile bool doLoop = false;

        public void start_processing() {
            doLoop = true;
            start();
        }

        public void stop_processing() {
            doLoop = false;
            stop();
        }

        protected override void thread_function() {

            Thread.CurrentThread.Name = string.Concat("ProcessData ", idD);
            Profiler.BeginThreadProfiling("ProcessData", Thread.CurrentThread.Name);
            while (doLoop) {
                Profiler.BeginSample("[DCNetworkDirectPlayer::process_data]");
                dllPlayer.process_data(idD);
                Profiler.EndSample();
            }
            Profiler.EndThreadProfiling();
        }
    }
}