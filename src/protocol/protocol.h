#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <vector>
#include "protocolenum.h"

class Protocol {
public:
    virtual ~Protocol() = default;

    virtual void handleProtocolCommand(const std::vector<uint8_t>& message) = 0;

protected:
    static ProtocolCommand extractCommandFromMessage(const std::vector<uint8_t>& message);
};

#endif // PROTOCOL_H
