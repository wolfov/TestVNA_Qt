#include "InteractionSVNA/InteractionSVNA.h"

void InteractionSVNA::Connect()
{
    emit sgnl_connect();
}

void InteractionSVNA::PrepairToWork()
{
    ResetSVNA();
    SetSource();
}

void InteractionSVNA::ConnectSlotsSignals()
{
    connect(this, &InteractionSVNA::sgnl_connect,
            _commSVNA,&CommunicationSVNA::connect);

    connect(_commSVNA, &CommunicationSVNA::error,
            this, &InteractionSVNA::sgnl_error);

    connect(_commSVNA, &CommunicationSVNA::sigDeviceConnected,
            this, &InteractionSVNA::slt_DeviceConnected);
}

void InteractionSVNA::slt_DeviceConnected(bool devOn, int name, QString idn)
{
    if(devOn)
        PrepairToWork();
}



