#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <vector>
#include "protocolenum.h"

class Protocol {
public:
    virtual ~Protocol() = default;

    virtual void processMessage(const std::vector<uint8_t>& message) = 0;

protected:
    ProtocolCommand readCommand(const std::vector<uint8_t>& message);
};

#endif // PROTOCOL_H
