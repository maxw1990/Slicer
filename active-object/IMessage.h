#pragma once
// Nachrichtenklasse
class Message
{
public:
    virtual bool execute() = 0;

    virtual ~Message() {}
};
