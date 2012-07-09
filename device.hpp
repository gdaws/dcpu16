#ifndef DCPU16_DEVICE_HPP
#define DCPU16_DEVICE_HPP

class device{
public:
    struct info{
        processor::dword hardware_id;
        processor::dword manufacturer_id;
        processor::word version;
    };
    
    virtual device::info get_info()const=0;
    virtual void interrupt(processor *)=0;
};

#endif
