#include <iostream>

#include <fstream>

#include <cstring>

#include <string>

#include <queue>

#include <deque>

#include <algorithm>

#include <cstdio>

#include <iterator>

using namespace std;

void fifo(char* tracefile, unsigned int nframes, char* option);
void lru(char* tracefile, unsigned int nframes, char* option);
void segmented_fifo(char* tracefile, unsigned int nframes, char* option, int percent);
//Data type for each Page Entry
struct pageEntry {
    unsigned addr;
    char rw;
    bool isDirty = false;
    pageEntry(unsigned addr, char rw) {
        this->addr = addr;
        this->rw = rw;
    }
    //overloaded operator used to compare two pages
    bool operator == (pageEntry cmp) {
        if (cmp.addr == this->addr)
            return true;
        else
            return false;
    }
};

int main(int argc, char* argv[]) {
    char* tracefile;
    char* nframes;
    char* replaceAlgo;
    char* option;
    char* percent;
    int per = 0;
    unsigned int numFrames;
    tracefile = strdup(argv[1]);
    nframes = strdup(argv[2]);
    numFrames = atoi(nframes);
    replaceAlgo = strdup(argv[3]);
    //get additional argument (percent) if the replacement policy is vms (segmented fifo)
    if (strcmp(replaceAlgo, "vms") == 0) {
        percent = strdup(argv[4]);
        per = atoi(percent);
        option = strdup(argv[5]);
    }
    else {
        option = strdup(argv[4]);
    }
    cout << endl << "******************************Memory Simulator******************************" << endl;
    cout << "Input parameters:" << endl;
    cout << "Trace Filename: " << tracefile << endl << "Number of frames: " << numFrames << endl << "Replacement policy: " << replaceAlgo << endl << "Output mode: " << option << endl << endl;
    if (strcmp(replaceAlgo, "fifo") == 0) {
        fifo(tracefile, numFrames, option);
    }
    else if (strcmp(replaceAlgo, "lru") == 0) {
        lru(tracefile, numFrames, option);
    }
    else if (strcmp(replaceAlgo, "vms") == 0) {
        segmented_fifo(tracefile, numFrames, option, per);
    }
    else {
        cout << "Invalid argument number 3, choose form fifo, lru, or vms" << endl;
    }

    cout << "****************************************************************************" << endl;
    return 0;

}

/*Fifo uses deque to store the page table. To implement FIFO elements are only added to the back
and only removed form the front.*/
void fifo(char* tracefile, unsigned int nframes, char* option) {
    int pageFaults = 0;
    int memReads = 0;
    int memWrites = 0;
    int events = 0;
    deque < pageEntry > buffer;
    FILE* file = fopen(tracefile, "r");
    while (!feof(file)) {
        pageEntry temp(0, ' ');
        unsigned int addr;
        char rw;
        int check = fscanf(file, "%x %c", &addr, &rw);
        temp.addr = addr / 4096; // gets the page number, as each page is of the size 4096
        temp.rw = rw;
        if (rw == 'W') {
            temp.isDirty = true;
        }
        if (check <= 0) { //checks if the file had an empty line
            continue;
        }
        events++;

        if (find(buffer.begin(), buffer.end(), temp) == buffer.end()) { //if the page is not in page table
            pageFaults++;
            if (strcmp(option, "debug") == 0) {
                cout << "Page Fault:-- ";
            }
            memReads++;
            if (strcmp(option, "debug") == 0) {
                cout << "page read from the disk: " << temp.addr << " ";
            }
            buffer.push_back(temp);
            if (buffer.size() > nframes) {
                pageEntry temp2 = buffer.front();
                if (strcmp(option, "debug") == 0) {
                    cout << "page evicted: " << temp2.addr << " ";
                }
                if (temp2.isDirty == true) {
                    memWrites++;
                    if (strcmp(option, "debug") == 0) {
                        cout << "written to disk" << memWrites;
                    }
                }
                buffer.pop_front();
            }
            if (strcmp(option, "debug") == 0) {
                cout << endl;
            }
        }
        else { //if page is in the page table
            deque < pageEntry > ::iterator up = (find(buffer.begin(), buffer.end(), temp));

            if (temp.rw == 'W') {
                (*up).isDirty = true;
            }
            if (strcmp(option, "debug") == 0) {
                cout << "Page updated in the memory: " << temp.addr << endl;
            }
        }

    }
    if (strcmp(option, "quiet") == 0 || strcmp(option, "debug") == 0) {
        cout << endl << "Total memory frames: " << nframes << endl;
        cout << "Events in trace: " << events << endl;
        cout << "Total disk reads: " << memReads << endl;
        cout << "Total disk writes: " << memWrites << endl;
    }
}

/*LRU is also implemented using a deque. The deque is updated every time there is no eviction to
make sure that the most recently used page is at the end and the least recently used will be at the front */
void lru(char* tracefile, unsigned int nframes, char* option) {
    int pageFaults = 0;
    int memReads = 0;
    int memWrites = 0;
    int events = 0;
    deque < pageEntry > buffer;
    FILE* file = fopen(tracefile, "r");
    while (!feof(file)) {
        pageEntry temp(0, ' ');
        unsigned int addr;
        char rw;
        int check = fscanf(file, "%x %c", &addr, &rw);
        temp.addr = addr / 4096; // gets the page number, as each page is of the size 4096
        temp.rw = rw;
        if (rw == 'W') {
            temp.isDirty = true;
        }
        //file >> temp.addr >> temp.rw;
        if (check <= 0) { //checks if the file had an empty line
            continue;
        }
        events++;

        if (find(buffer.begin(), buffer.end(), temp) == buffer.end()) { //if the page is not in page table
            pageFaults++;
            if (strcmp(option, "debug") == 0) {
                cout << "Page Fault:-- ";
            }
            memReads++;
            if (strcmp(option, "debug") == 0) {
                cout << "page read from the disk: " << temp.addr << " ";
            }
            buffer.push_back(temp);
            if (buffer.size() > nframes) {
                pageEntry temp2 = buffer.front();
                if (strcmp(option, "debug") == 0) {
                    cout << "page evicted: " << temp2.addr << " ";
                }
                if (temp2.isDirty == true) {
                    memWrites++;
                    if (strcmp(option, "debug") == 0) {
                        cout << "written to disk" << memWrites;
                    }
                }
                buffer.pop_front();
            }
            if (strcmp(option, "debug") == 0) {
                cout << endl;
            }
        }
        else { //if page is in the page table

            deque < pageEntry > ::iterator up = (find(buffer.begin(), buffer.end(), temp));
            if (temp.rw == 'W') {
                (*up).isDirty = true;
            }
            temp.isDirty = (*up).isDirty;
            buffer.erase(up);
            buffer.push_back(temp);

            if (strcmp(option, "debug") == 0) {
                cout << "Page already in the memory, shifted to the end (to be removed last): " << temp.addr << endl;
            }
        }

    }
    if (strcmp(option, "quiet") == 0 || strcmp(option, "debug") == 0) {
        cout << endl << "Total memory frames: " << nframes << endl;
        cout << "Events in trace: " << events << endl;
        cout << "Total disk reads: " << memReads << endl;
        cout << "Total disk writes: " << memWrites << endl;
    }
}
void segmented_fifo(char* tracefile, unsigned int nframes, char* option, int percent) {
    unsigned int secBuffSize = (nframes * percent) / 100;
    unsigned int primBufferSize = nframes - secBuffSize;
    int pageFaults = 0;
    int memReads = 0;
    int memWrites = 0;
    int events = 0;
    deque < pageEntry > primBuffer;
    deque < pageEntry > secBuffer;
    FILE* file = fopen(tracefile, "r");
    while (!feof(file)) {
        pageEntry temp(0, ' ');
        unsigned int addr;
        char rw;
        int check = fscanf(file, "%x %c", &addr, &rw);
        temp.addr = addr / 4096; // gets the page number, as each page is of the size 4096
        temp.rw = rw;
        if (rw == 'W') {
            temp.isDirty = true;
        }
        if (check <= 0) { //checks if the file had an empty line
            continue;
        }
        events++;
        if (find(primBuffer.begin(), primBuffer.end(), temp) == primBuffer.end()) { // if the page is not in the primary
            if (find(secBuffer.begin(), secBuffer.end(), temp) != secBuffer.end()) // if the page is in the secondary
            {
                deque < pageEntry > ::iterator up = (find(secBuffer.begin(), secBuffer.end(), temp));
                if ((*up).isDirty == true) {
                    temp.isDirty = (*up).isDirty;
                }
                secBuffer.erase(up);
                if (strcmp(option, "debug") == 0) {
                    cout << "Page removed the secondary buffer, added to the primary buffer: " << temp.addr << " ";
                }
                primBuffer.push_back(temp);
                if (primBuffer.size() > primBufferSize) {
                    pageEntry temp2 = primBuffer.front();
                    primBuffer.pop_front();
                    if (strcmp(option, "debug") == 0) {
                        cout << "Page evicted from primary buffer and added to secondary buffer: " << temp2.addr << " ";
                    }
                    secBuffer.push_back(temp2);
                    if (secBuffer.size() > secBuffSize) {
                        pageEntry temp2 = secBuffer.front();
                        if (strcmp(option, "debug") == 0) {
                            cout << "Page evicted from secondary buffer: " << temp2.addr << " ";
                        }
                        if (temp2.isDirty == true) {
                            memWrites++;
                            if (strcmp(option, "debug") == 0) {
                                cout << "written to disk";
                            }
                        }
                        secBuffer.pop_front();
                    }

                }
                if (strcmp(option, "debug") == 0) {
                    cout << endl;
                }
            }
            else { // if the page is not in the secondary
                pageFaults++;
                if (strcmp(option, "debug") == 0) {
                    cout << "Page Fault:-- ";
                }
                memReads++;
                if (strcmp(option, "debug") == 0) {
                    cout << "page read from the disk: " << temp.addr << " ";
                }

                primBuffer.push_back(temp);
                if (primBuffer.size() > primBufferSize) {
                    pageEntry temp2 = primBuffer.front();
                    primBuffer.pop_front();
                    if (strcmp(option, "debug") == 0) {
                        cout << "Page evicted from primary buffer and added to secondary buffer: " << temp2.addr << " ";
                    }
                    secBuffer.push_back(temp2);
                    if (secBuffer.size() > secBuffSize) {
                        pageEntry temp2 = secBuffer.front();
                        if (strcmp(option, "debug") == 0) {
                            cout << "Page evicted from secondary buffer: " << temp2.addr << " ";
                        }
                        if (temp2.isDirty == true) {
                            memWrites++;
                            if (strcmp(option, "debug") == 0) {
                                cout << "written to disk";
                            }
                        }
                        secBuffer.pop_front();
                    }
                }
                if (strcmp(option, "debug") == 0) {
                    cout << endl;
                }
            }
        }
        else { //if page is in the page table
            deque < pageEntry > ::iterator up = (find(primBuffer.begin(), primBuffer.end(), temp));

            if (temp.rw == 'W') {
                (*up).isDirty = true;
            }
            if (strcmp(option, "debug") == 0) {
                cout << "Page updated in the memory: " << temp.addr << endl;
            }
        }

    }
    if (strcmp(option, "quiet") == 0 || strcmp(option, "debug") == 0) {
        cout << endl << "Total memory frames: " << nframes << endl;
        cout << "Events in trace: " << events << endl;
        cout << "Total disk reads: " << memReads << endl;
        cout << "Total disk writes: " << memWrites << endl;
    }

}