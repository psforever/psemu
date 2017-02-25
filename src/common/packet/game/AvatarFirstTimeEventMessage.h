#pragma once

#include "common/packet/opcodes.h"
#include "common/bitstream.h"

/**
    Dispatched to the server when the player encounters something for the very first time in their campaign.
    For example, the first time the player rubs up against a game object with a yellow exclamation point.
    For example, the first time the player draws a specific weapon.

    When the first time events (FTE's) are received, battle experience is awarded.
    Text information about the object will be displayed.
    A certain itemized checkbox under the 'Training' tab that corresponds is marked off.
    The latter list indicates all "encounterable" game objects for which a FTE exists.
    These effects only happen once per character/campaign.
    (The Motion Sensor will occasionally erronously display the information window on repeat encounters.
    No additional experience, though.)

    First time events (FTE's) are recorded in a great list in the middle of player ObjectCreateMessage data.
    Tutorial complete events are enqueued nearby.
    @property avatar_uid the player
    @property object_id the game object that triggers the event
    @property unk na
    @property event_name the string name of the event
*/
class AvatarFirstTimeEventMessage {
public:
    uint16_t avatar_uid;
    uint16_t object_id;
    uint32_t unk;
    std::string event_name;

    static AvatarFirstTimeEventMessage decode(BitStream &bitstream) {
        AvatarFirstTimeEventMessage packet;
        bitstream.read(packet.avatar_uid);
        bitstream.read(packet.object_id);
        bitstream.read(packet.unk);
        bitstream.read(packet.event_name);
        return packet;
    }
};
