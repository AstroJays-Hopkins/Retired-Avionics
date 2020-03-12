
/** This struct represents a comamnd, and contains the comamnd code, state and
 * last read time
 */
struct Command {
    int8_t code;
    int8_t value;
    uint8_t retries;
    unsigned long lastTrans;
}


class CommandQueue { 
    private:
        Command commands[MAX_CQ_SIZE];
        // we should use size_t, but we don't because memory is limited on these
        // puny things
        uint8_t head;
        uint8_t tail;
        bool full;
    public:
        CommandQueue();
        void put(int8_t code, int8_t value, uint8_t retries, unsigned long lastTrans);
        Command* get();
        // mark this function as inlineable by defining it in the class 
        bool isEmpty() { return !full && head == tail; }
        uint8_t send();
}

CommandQueue::CommandQueue() {
    head = 0;
    tail = 0;
}

void CommandQueue::put(int8_t code, int8_t value, uint8_t retries, 
                  unsigned long lastTrans) {
    commands[head].code = code;
    commands[head].value = value;
    commands[head].retries = retries;
    commands[head].lastTrans = lastTrans;
    if(full) {
        // bitwise & (<val>-1) is the same as mod val becuase of how binary
        // bitwise ops work
        tail = (tail + 1) & (MAX_CQ_SIZE - 1);
    }
    head = (head + 1) & (MAX_CQ_SIZE - 1);
    full = head == tail;
}

Command* CommandQueue:get() {
    if(isEmpty()) {
        return nullptr;
    }
    Command* data = &commands[tail];
    tail = (tail + 1) & (MAX_CQ_SIZE - 1);
    return data;
}

uint8_t CommandQueue::send(int timeint, unsigned long now) {
    if(isEmpty()) {
        return 0;
    }
    uint8_t status = 0;
    if(commands[tail].retries > MAX_CQ_RETRIES) {
        get();
        status = 1;
    }
    if(now - comamnds[tail].lastTrans > timeint) {
        LoRa.beginPacket();
        LoRa.write(1);
        LoRa.write(commands[tail].code);
        LoRa.write(comamnds[tail].data);
        LoRa.endPacket();
        commands[tail].lastTrans = now;
        ++commands[tail].retries;
    }
    return status;
}


