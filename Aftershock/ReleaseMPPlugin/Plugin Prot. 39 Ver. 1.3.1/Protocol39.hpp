/*
Copyright (c) 2012, Mischa Aster Alff && Adam Guy && NotMeh Kamilla (I don't know her name. :X)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Aftershock nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ADAM GUY BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef RELEASEPROTOCOL39_HPP_INCLUDED
#define RELEASEPROTOCOL39_HPP_INCLUDED

#include "Packets.hpp"

typedef int8_t byte;
typedef uint8_t ubyte;
typedef std::string metadata;
typedef int NEED_TO_CHANGE;
/*
#define C_BLACK "\u00A70"
#define C_DBLUE "\u00A71"
#define C_DGREEN "\u00A72"
#define C_DCYAN "\u00A73"
#define C_DRED "\u00A74"
#define C_PURPLE "\u00A75"
#define C_GOLD "\u00A76"
#define C_GREY "\u00A77"
#define C_DGREY "\u00A78"
#define C_BLUE "\u00A79"
#define C_BGREEN "\u00A7a"
#define C_CYAN "\u00A7b"
#define C_RED "\u00A7c"
#define C_PINK "\u00A7d"
#define C_YELLOW "\u00A7e"
#define C_WHITE "\u00A7f"
#define C_RESET "\u00A7r"
#define S_RANDOM "\u00A7k"
#define S_BOLD "\u00A7l"
#define S_STRIKEOUT "\u00A7m"
#define S_UNDERLINE "\u00A7n"
#define S_ITALIC "\u00A7o"
#define S_RESET "\u00A7r"
*/
/*
class SlotData
{
    public:
        std::string SlotDatastd::string;
        
        SlotData(std::string SlotDatastd::string_)
        {
            SlotDatastd::string = SlotDatastd::string_;
        }
        SlotData(const SlotData &Source)
        {
            SlotDatastd::string = Source.SlotDatastd::string;
        }
        
        SlotData& operator= (const SlotData &source);
};

SlotData& SlotData::operator= (const SlotData &Source)
{
    SlotDatastd::string = Source.SlotDatastd::string;
    static char ByteArray = new static char [Source.SlotDatastd::string.size()];
    ByteArray = Source.SlotDatastd::string.c_str;
    return *this;
}
*/
class SlotData
{
    public:
        short length;
        void define(std::string data)
        {
           static char ByteArray = new static char [data.size()];
           ByteArray = data.c_str;
           length=data,size();
        }
};

namespace Client_Packets
{
    enum: uint8_t { Info, Blockchange = 5, Movement = 8, Chat = 13 };

    struct KeepAlive0x00 : Packet<\x00,int>::DynamicLength
    {
        enum:uint32_t
        {
            keepaliveid
        };
    };
    
    struct Handshake0x02 : Packet<\x02,byte,std::string,std::string,int>::DynamicLength
    {
        enum:uint32_t
        {
            protocolversion,
            username,
            serverhost,
            serverport
        };
    };
    
    struct ChatMessage0x03 : Packet<\x03,std::string>::DynamicLength
    {
        enum:uint32_t
        {
            message
        };
    };
    
    struct UseEntity0x07 : Packet<\x07,int,int,bool>::DynamicLength
    {
        enum:uint32_t
        {
            user,
            target,
            mouse_button
        };
    };
    
    struct Player0x0A : Packet<\x0A,bool>::DynamicLength
    {
        enum:int8_t
        {
            on_ground
        };
    };
    
    struct PlayerPosition0x0B : Packet<\x0B,double,double,double,double,bool>::DynamicLength
    {
        enum:int32_t
        {
            x,
            y,
            stance,
            z,
            on_ground
        };
    };
    
    struct PlayerLook0x0C : Packet<\x0C,float,float,bool>::DynamicLength
    {
        enum:int32_t
        {
            yaw,
            pitch,
            on_ground
        };
    };
    
    struct PlayerPosLook0x0D : Packet<\x0D,double,double,double,double,float,float,bool>::DynamicLength
    {
        enum:int32_t
        {
            x,
            y,
            stance,
            z,
            yaw,
            pitch,
            on_ground
        };
    };
    
    struct PlayerDigging0x0E : Packet<\x0E,byte,int,byte,int,byte>::DynamicLength
    {
        enum:int32_t
        {
            status,
            x,
            y,
            z,
            face
        };
    };
    
    struct PlayerBlockPlacement0x0F : Packet<\x0F,int,ubyte,int,byte,SlotData,byte,byte,byte>::DynamicLength
    {
        enum:int32_t
        {
            x,
            y,
            z,
            direction,
            held_item,
            cursor_pos_x,
            cursor_pos_y,
            cursor_pos_z
        };
    };
    
    struct HeldItemChange0x10 : Packet<\x10,short>::DynamicLength
    {
        enum:int32_t
        {
            slotid
        };
    };
    
    struct Animation0x12 : Packet<\x12,int,byte>::DynamicLength
    {
        enum:int32_t
        {
            eid,
            animation
        };
    };
    
    struct EntityAction0x13 : Packet<\x13,int,byte>::DynamicLength
    {
        enum:int32_t
        {
            eid,
            actionid
        };
    };
    
	struct CloseWindow0x65 : Packet<\x65,byte>::DynamicLength
	{
		enum:int32_t
		{
			window_id,
		}
	}
	
	struct ClickWindow0x66 : Packet<\x66,byte,short,byte,short,bool,SlotData>::DynamicLength
	{
		enum:int32_t
		{
			window_id,
			slot,
			right_click,
			action_number,
			shift,
			clicked_item
		}
	}
	
		struct ConfirmTransaction0x6A : Packet<\x6A,byte,short,bool>::DynamicLength
	{
		enum:int32_t
		{
			window_id,
			action_number,
			accepted?
		}
	}
	
	struct CreativeInventoryAction0x6B : Packet<\x6B,short,SlotData>::DynamicLength
	{
		enum:int32_t
		{
			slot,
			clicked_item
		}
	}
	
	struct EnchantItem0x6C : Packet<\x6C,byte,byte>::DynamicLength
	{
		enum:int32_t
		{
			window_id,
			enchantment
		}
	}
	
	struct UpdateSign0x82 : Packet<\x82,int,short,int,std::string,std::string,std::string,std::string>::DynamicLength
	{
		enum:int32_t
		{
			x,
			y,
			z,
			text1,
			text2,
			text3,
			text4
		}
	}
	
	struct PlayerAbilities0xCA : Packet<\xCA,byte,byte,byte>::DynamicLength
	{
		enum:int32_t
		{
			flags,
			flying_speed,
			walking_speed,
		}
	}
	
	struct TabComplete0xCB : Packet<\xCB,std::string>::DynamicLength
	{
		enum:int32_t
		{
			text
		}
	}
	
	struct LocaleandViewDistance0xCC : Packet<\xCC,std::string,byte,byte,byte>::DynamicLength
	{
		enum:int32_t
		{
			locale,
			view_distance,
			chat_flags,
			difficulty
		}
	}
	
	struct ClientStatuses0xCD : Packet<\xCD,byte>::DynamicLength
	{
		enum:int32_t
		{
			payload
		}
	}
	
	struct PluginMessage0xFA : Packet<\xFA,std::string,short,byte[]>::DynamicLength
	{
		enum:int32_t
		{
			channel,
			length,
			data
		}
	}
	
	struct EncryptioionKeyResponse0xFC : Packet<\xFC,short,byte[],short,byte[]>::DynamicLength
	{
		enum:int32_t
		{
			shared_secret_length,
			shared_secret,
			verify_token_length,
			verify_token_response
		}
	}
	
	struct ServerListPing0xFE : Packet<\xFE>::DynamicLength 
	{
	}
	
	struct DisconnectKick0xFF : Packet<\xFF,std::string>::DynamicLength
	{
		enum:int32_t
		{
			reason
		}
	}
}

namespace Server_Packets
{
    struct KeepAlive0x00 : Packet<\x00,int32_t>::DynamicLength
    {
        enum:uint32_t
        {
            keepaliveid
        };
    };
    
    struct LoginRequest0x01 : Packet<\x01,int32_t,std::string,byte,byte,byte,ubyte,ubyte>::DynamicLength
    {
        enum:uint32_t
        {
            entityid,
            leveltype,
            gamemode,
            dimension,
            difficulty,
            worldheight,
            maxplayers
        };
    };
    
    struct ChatMessage0x03 : Packet<\x03,std::string>::DynamicLength
    {
        enum:uint32_t
        {
            message
        };
    };
    
    struct TimeUpdate0x04 : Packet<\x04,long>::DynamicLength
    {
        enum:uint32_t
        {
            time
        };
    };
    
    struct EntityEquipment0x05 : Packet<\x05,int,short,SlotData>::DynamicLength
    {
        enum:uint32_t
        {
            entityid,
            slot,
            item
        };
    };
    
    struct SpawnPosition0x06 : Packet<\x06,int,int,int>::DynamicLength
    {
        enum:uint32_t
        {
            x,
            y,
            z
        };
    };
    
    struct UpdateHealth0x08 : Packet<\x08,short,short,float>::DynamicLength
    {
        enum:uint32_t
        {
            health,
            food,
            food_saturation
        };
    };
    
    struct Respawn0x09 : Packet<\x09,int,byte,byte,short,std::string>::DynamicLength
    {
        enum:uint32_t
        {
            dimension,
            difficulty,
            gamemode,
            world_height,
            level_type
        };
    };
    
    struct PlayerPosLook0x0D : Packet<\x0D,double,double,double,double,float,float,bool>::DynamicLength
    {
        enum:int32_t
        {
            x,
            stance,
            y,
            z,
            yaw,
            pitch,
            on_ground
        };
    };
    
    struct UseBed0x11 : Packet<\x11,int,byte,int,byte,int>::DynamicLength
    {
        enum:int32_t
        {
            entityid,
            unknown,
            x_coord,
            y_coord,
            z_coord
        };
    };
    
    struct Animation0x12 : Packet<\x12,int,byte>::DynamicLength
    {
        enum:int32_t
        {
            eid,
            animation
        };
    };
    
    struct SpawnNamedEntity0x14 : Packet<\x14,int,std::string,int,int,int,byte,byte,short,metadata>::DynamicLength
    {
        enum:int32_t
        {
            eid,
            player_name,
            x,
            y,
            z,
            yaw,
            pitch,
            current_item,
            metadata
        };
    };
    
    struct SpawnDroppedItem0x15 : Packet<\x15,int,short,byte,short,int,int,int,byte,byte,byte>::DynamicLength
    {
        enum:int32_t
        {
			eid,
			item,
			count,
			damagedata,
			x,
			y,
			z,
			rotation,
			pitch,
			roll
        }
    }
	
	struct CollectItem0x16 : Packet<\x16,int,int>::DynamicLength
	{
		enum:int32_t
		{
			collected_eid,
			collector_eid
		}
	}
	
	struct SpawnObject/Vehicle0x17 : Packet<\x17,int,byte,int,int,int,int,short,short>::DynamicLength
	{
		enum:int32_t
		{
			eid,
			type,
			x,
			y,
			z,
			object_dara,
			speed_x,
			speed_y,
			speed_z
		}
	}
	
	struct SpawnMob0x18 : Packet<\x18,int,byte,int,int,int,byte,byte,byte,short,short,short,metadata>::DynamicLength
	{
		enum:int32_t
		{
			eid,
			type,
			x,
			y,
			z,
			yaw,
			pitch,
			head_yaw,
			velocity_z,
			velocity_x,
			velocity_y,
			metadata
		}
	}
	
	struct SpawnPainting0x19 : Packet<\x19,int,std::string,int,int,int,int>::DynamicLength
	{
		enum:int32_t
		{
			entity_id,
			title,
			x,
			y,
			z,
			direction
		}
	}
	
	struct SpawnExperienceOrb0x1A : Packet<\x1A,int,int,int,int,short>::DynamicLength
	{
		enum:int32_t
		{
			entity_id,
			x,
			y,
			z,
			count
		}
	}
	
	struct EntityVelocity0x1C : Packet<\x1C,int,short,short,short>::DynamicLength
	{
		enum:int32_t
		{
			entity_id,
			velocity_x,
			velocity_y,
			velocity_z
		}
	}
	
	struct DestroyEntity0x1D : Packet<\x1D,byte,int[]>::DynamicLength
	{
		enum:int32_t
		{
			entity_count,
			entity_ids
		}
	}
	
	struct Entity0x1E : Packet<\x1E,int>::DynamicLength
	{
		enum:int32_t
		{
			eid
		}
	}
	
	struct EntityRelativeMove0x1F : Packet<\x1F,int,byte,byte,byte>::DynamicLength
	{
		enum:int32_t
		{
			eid,
			dx,
			dy,
			dz
		}
	}
	
	struct EntityLookandRelativeMove0x21 : Packet<\x21,int,byte,byte,byte,byte,byte>::DynamicLength
	{
		enum:int32_t
		{
			eid,
			dx,
			dy,
			dz,
			yaw,
			pitch
		}
	}

	struct EntityTeleport0x22 : Packet<\x22,int,int,int,int,byte,byte>::DynamicLength
	{
		enum:int32_t
		{
			eid,
			x,
			y,
			z,
			yaw,
			pitch
		}
	}
	
	struct EntityHeadLook0x23 : Packet<\x23,int,byte>::DynamicLength
	{
		enum:int32_t
		{
			entity_id,
			head_yaw
		}
	}
	
	struct EntityStatus0x26 : Packet<\x26,int,byte>::DynamicLength
	{
		enum:int32_t
		{
			entity_id,
			entity_status
		}
	}
	
	struct AttachEntity0x27 : Packet<\x27,int,int>::DynamicLength
	{
		enum:int32_t
		{
			entity_id,
			vehicle_id
		}
	}
	
	struct EntityMetadata0x28 : Packet<\x28,int,metadata>::DynamicLength
	{
		enum:int32_t
		{
			entity_id,
			entity_metadata
		}
	}
	
	struct EntityEffect0x29 : Packet<\x29,int,byte,byte,short>::DynamicLength
	{
		enum:int32_t
		{
			entity_id,
			effect_id,
			amplifier,
			duration
		}
	}
	
	struct RemoveEntityEffect0x2A : Packet<\x2A,int,byte>::DynamicLength
	{
		enum:int32_t
		{
			entity_id,
			effect_id
		}
	}
	
	struct SetExperience0x2B : Packet<\x2B,float,short,short>::DynamicLength
	{
		enum:int32_t
		{
			experience_bar,
			level,
			total_experience
		}
	}
	
	struct ChunkData0x33 : Packet<\x33,int,int,bool,unsigned short,unsigned short,int,ubyte[]>::DynamicLength
	{
		enum:int32_t
		{
			x,
			z,
			groundup_continuous,
			primary_bit_map,
			add_bit_map,
			compressed_size,
			compressed_data
		}
	}
	
	struct MultiBlockChange0x34 : Packet<\x34,int,int,short,int,NEED_TO_CHANGE>::DynamicLength
	{
		enum:int32_t
		{
			chunk_x,
			chunk_z,
			record_count,
			data_size,
			data
		}
	}
	
	struct BlockChange0x35 : Packet<\x35,int,byte,int,short,byte>::DynamicLength
	{
		enum:int32_t
		{
			x,
			y,
			z,
			block_type,
			block_metadata
		}
	}
	
	struct BlockAction0x36 : Packet<\x36,int,short,int,byte,byte,short>::DynamicLength
	{
		enum:int32_t
		{
			x,
			y,
			z,
			byte_1,
			byte_2,
			block_id
		}
	}
	
	struct Explosion0x3C : Packet<\x3C,double,double,double,float,int,NEED_TO_CHANGE,float,float,float>::DynamicLength
	{
		enum:int32_t
		{
			x,
			y,
			z,
			radius,
			record_count,
			records,
			unknown,
			unknown,
			unknown
		}
	}
	
	struct SoundsParticleEffect0x3D : Packet<\x3D,int,int,byte,int,int>::DynamicLength
	{
		enum:int32_t
		{
			effect_id,
			x,
			y,
			z,
			data
		}
	}
	
	struct NamedSoundEffect0x3E : Packet<\x3E,std::string,int,int,int,float,byte>::DynamicLength
	{
		enum:int32_t
		{
			sound_name,
			effect_pos_x,
			effect_pos_y,
			effect_pos_z,
			volume,
			pitch
		}
	}
	
	struct ChangeGameState0x46 : Packet<\x46,byte,byte>::DynamicLength
	{
		enum:int32_t
		{
			reason,
			game_mode
		}
	}
	
	struct Thunderbolt0x47 : Packet<\x47,int,bool,int,int,int>::DynamicLength
	{
		enum:int32_t
		{
			entity_id,
			unknown,
			x,
			y,
			z
		}
	}
	
	struct OpenWindow0x64 : Packet<\x64,byte,byte,std::string,byte>::DynamicLength
	{
		enum:int32_t
		{
			window_id,
			inventory_type,
			window_title,
			number_of_slots
		}
	}
	
	struct CloseWindow0x65 : Packet<\x65,byte>::DynamicLength
	{
		enum:int32_t
		{
			window_id,
		}
	}
	
	struct SetSlot0x67 : Packet<\x67,byte,short,SlotData>::DynamicLength
	{
		enum:int32_t
		{
			window_id,
			slot,
			slot_data,
		}
    }
	
	struct SetWindowItems0x68 : Packet<\x68,byte,short,SlotData[]>::DynamicLength
	{
		enum:int32_t
		{
			window_id,
			count,
			slot_data
		}
	}
	
	struct UpdateWindowPropety0x69 : Packet<\x69,byte,short,short>::DynamicLength
	{
		enum:int32_t
		{
			window_id,
			property,
			value
		}
	}
	
	struct ConfirmTransaction0x6A : Packet<\x6A,byte,short,bool>::DynamicLength
	{
		enum:int32_t
		{
			window_id,
			action_number,
			accepted?
		}
	}
	
	struct CreativeInventoryAction0x6B : Packet<\x6B,short,SlotData>::DynamicLength
	{
		enum:int32_t
		{
			slot,
			clicked_item
		}
	}
	
	struct UpdateSign0x82 : Packet<\x82,int,short,int,std::string,std::string,std::string,std::string>::DynamicLength
	{
		enum:int32_t
		{
			x,
			y,
			z,
			text1,
			text2,
			text3,
			text4
		}
	}
	
	struct ItemData0x83 : Packet<\x83,short,short,ubyte,byte[]>::DynamicLength
	{
		enum:int32_t
		{
			item_type,
			item_id,
			text_length,
			text
		}
	}
	
	struct UpdateTileEntity0x84 : Packet<\x84,int,short,int,byte,short,NEED_TO_CHANGE>::DynamicLength
	{
		enum:int32_t
		{
			x,
			y,
			z,
			action,
			data_length,
			nbt_data
		}
	}
	
	struct IncrementStatistic0xC8 : Packet<\xC8,int,byte>::DynamicLength
	{
		enum:int32_t
		{
			statistic_id,
			amount
		}
	}
	
	struct PlayerListItem0xC9 : Packet<\xC9,std::string,bool,short>::DynamicLength
	{
		enum:int32_t
		{
			player_name,
			online,
			ping
		}
	}
	
	struct PlayerAbilities0xCA : Packet<\xCA,byte,byte,byte>::DynamicLength
	{
		enum:int32_t
		{
			flags,
			flying_speed,
			walking_speed,
		}
	}
	
	struct TabComplete0xCB : Packet<\xCB,std::string>::DynamicLength
	{
		enum:int32_t
		{
			text
		}
	}
	
	struct PluginMessage0xFA : Packet<\xFA,std::string,short,byte[]>::DynamicLength
	{
		enum:int32_t
		{
			channel,
			length,
			data
		}
	}
	
	struct EncryptioionKeyResponse0xFC : Packet<\xFC,short,byte[],short,byte[]>::DynamicLength
	{
		enum:int32_t
		{
			shared_secret_length,
			shared_secret,
			verify_token_length,
			verify_token_response
		}
	}
	
	struct EncryptionKeyRequest0xFD : Packet<\xFD,std::string,short,byte[],short,byte[]>::DynamicLength
	{
		enum:int32_t
		{
			server_id,
			public_key_length,
			public_key,
			verify_token_length,
			verify_token
		}
	}
	
	struct DisconnectKick0xFF : Packet<\xFF,std::string>::DynamicLength
	{
		enum:int32_t
		{
			reason
		}
	}
	
}

namespace UNKNOWN
{		
	struct BlockBreakAnimation0x37 : Packet<\x37,int,int,int,int,byte>::DynamicLength
	{
		enum:int32_t
		{
			eid,
			x,
			y,
			z,
			destroy_stage,
		}
	}
	
	struct MapChunkBulk0x38 : Packet<\x38,short,int,byte[],NEED_TO_CHANGE>::DynamicLength
	{
		enum:int32_t
		{
			chunk_column_count,
			chunk_data_size,
			data,
			meta_information
		}
	}
}

enum MAP_BLOCK :
uint8_t
{
    BLOCK_AIR,
    BLOCK_STONE,
    BLOCK_GRASS,
    BLOCK_DIRT,
    BLOCK_COBBLESTONE,
    BLOCK_WOOD,
    BLOCK_SAPLING,
    BLOCK_ADMINIUM,
    BLOCK_WATER,
    BLOCK_STATIC_WATER,
    BLOCK_LAVA,
    BLOCK_STATIC_LAVA,
    BLOCK_SAND,
    BLOCK_GRAVEL,
    BLOCK_GOLD_ORE,
    BLOCK_IRON_ORE,
    BLOCK_COAL_ORE,
    BLOCK_TREE,
    BLOCK_LEAVES,
    BLOCK_SPONGE,
    BLOCK_GLASS,
    BLOCK_RED_CLOTH,
    BLOCK_ORANGE_CLOTH,
    BLOCK_YELLOW_CLOTH,
    BLOCK_LIGHT_GREEN_CLOTH,
    BLOCK_GREEN_CLOTH,
    BLOCK_AQUA_GREEN_CLOTH,
    BLOCK_CYAN_CLOTH,
    BLOCK_BLUE_CLOTH,
    BLOCK_PURPLE_CLOTH,
    BLOCK_INDIGO_CLOTH,
    BLOCK_VIOLET_CLOTH,
    BLOCK_MEGENTA_CLOTH,
    BLOCK_PINK_CLOTH,
    BLOCK_BLACK_CLOTH,
    BLOCK_GREY_CLOTH,
    BLOCK_WHITE_CLOTH,
    BLOCK_FLOWER,
    BLOCK_ROSE,
    BLOCK_RED_MUSHROOM,
    BLOCK_BROWN_MUSHROOM,
    BLOCK_GOLD,
    BLOCK_IRON,
    BLOCK_DOUBLE_STAIR,
    BLOCK_STAIR,
    BLOCK_BRICK,
    BLOCK_TNT,
    BLOCK_BOOKCASE,
    BLOCK_MOSSY_COBBLESTONE,
    BLOCK_OBSIDIAN
};
#endif // RELEASEPROTOCOL39_HPP_INCLUDED
