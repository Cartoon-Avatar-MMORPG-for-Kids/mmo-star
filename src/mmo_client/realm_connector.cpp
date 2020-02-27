// Copyright (C) 2019, Robin Klimonow. All rights reserved.

#include "realm_connector.h"
#include "version.h"

#include "base/constants.h"
#include "base/random.h"
#include "base/sha1.h"
#include "log/default_log_levels.h"

#include <iomanip>

namespace mmo
{
	RealmConnector::RealmConnector(asio::io_service & io)
		: auth::Connector(std::make_unique<asio::ip::tcp::socket>(io), nullptr)
		, m_ioService(io)
		, m_serverSeed(0)
		, m_clientSeed(0)
	{
	}

	RealmConnector::~RealmConnector()
	{
	}

	PacketParseResult RealmConnector::OnAuthChallenge(mmo::auth::IncomingPacket & packet)
	{
		// No longer handle LogonChallenge packets during this session
		ClearPacketHandler(auth::realm_client_packet::AuthChallenge);

		// Try to read the packet data
		if (!(packet >> io::read<uint32>(m_serverSeed)))
		{
			return PacketParseResult::Disconnect;
		}

		// Calculate a hash for verification
		HashGeneratorSha1 hashGen;
		hashGen.update(m_account.data(), m_account.length());
		hashGen.update(reinterpret_cast<const char*>(&m_clientSeed), sizeof(m_clientSeed));
		hashGen.update(reinterpret_cast<const char*>(&m_serverSeed), sizeof(m_serverSeed));
		Sha1_Add_BigNumbers(hashGen, {m_sessionKey});
		SHA1Hash hash = hashGen.finalize();

		// We have been challenged, respond with an answer
		sendSinglePacket([this, &hash](auth::OutgoingPacket& packet) {
			packet.Start(auth::client_realm_packet::AuthSession);
			packet
				<< io::write<uint32>(mmo::Revision)
				<< io::write_dynamic_range<uint8>(this->m_account)
				<< io::write<uint32>(m_clientSeed)
				<< io::write_range(hash);
			packet.Finish();
		});

		// Debug log
		DLOG("[Realm] Handshaking...");

		// Proceed handling network packets
		return PacketParseResult::Pass;
	}

	bool RealmConnector::connectionEstablished(bool success)
	{
		if (success)
		{
			// Reset server seed
			m_serverSeed = 0;

			// Generate a new client seed
			std::uniform_int_distribution<uint32> dist;
			m_clientSeed = dist(RandomGenerator);

			// Accept LogonChallenge packets from here on
			RegisterPacketHandler(auth::realm_client_packet::AuthChallenge, *this, &RealmConnector::OnAuthChallenge);
		}
		else
		{
			ELOG("Could not connect to the realm server");
		}

		return true;
	}

	void RealmConnector::connectionLost()
	{
		// Log this event
		ELOG("Lost connection to the realm server...");

		// Clear packet handlers
		ClearPacketHandlers();
	}

	void RealmConnector::connectionMalformedPacket()
	{
		ELOG("Received a malformed packet");
	}

	PacketParseResult RealmConnector::connectionPacketReceived(auth::IncomingPacket & packet)
	{
		return HandleIncomingPacket(packet);
	}

	void RealmConnector::Connect(const std::string& realmAddress, uint16 realmPort, const std::string& accountName, const std::string& realmName, BigNumber sessionKey)
	{
		m_realmAddress = realmAddress;
		m_realmPort = realmPort;
		m_realmName = realmName;
		m_account = accountName;
		m_sessionKey = sessionKey;

		// Connect to the server
		connect(m_realmAddress, m_realmPort, *this, m_ioService);
	}
}
