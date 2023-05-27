#pragma once
#ifndef	UUID_H
#define UUID_H

#include "../Random/Random.h"

namespace BE
{
	//UUID used for identifying entities, assets, and scenes
	class BEID
	{
	public:
		using IDType = std::uint64_t;

	private:
		IDType m_id;

	public:
		BEID();												//Generate a random ID
		explicit BEID(IDType _id);							//Uses a predefined ID, such as through deserialisation. DOES NOT CHEK FOR COLLISION!
		BEID(const BEID& copy) = default;					//Creates a copy of the UUID.

		//Functions
		friend std::ostream& operator<<(std::ostream& os, const BEID& _id);		//Print ID to output stream
		operator IDType() const;												//Converts to integer for saving
		bool operator==(const BEID& _rhs) const;								//Compares 2 UUID
		bool operator!=(const BEID& _rhs) const;
		BEID& operator=(const BEID& _rhs) = default;
		bool operator<(const BEID& _rhs);
		inline IDType operator*() const { return m_id; }
#if 1
		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND
#endif
	};
}
template<>
struct std::hash<BE::BEID>
{
	size_t operator()(const BE::BEID& val) const
	{
		return std::hash<BE::BEID::IDType>()(val);
	}
};

#endif