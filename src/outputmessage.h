// Copyright 2023 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_OUTPUTMESSAGE_H
#define FS_OUTPUTMESSAGE_H

#include "networkmessage.h"
#include "connection.h"
#include "tools.h"

class OutputMessage : public NetworkMessage {
	public:
		OutputMessage() = default;

		// non-copyable
		OutputMessage(const OutputMessage&) = delete;
		OutputMessage& operator=(const OutputMessage&) = delete;

		uint8_t* getOutputBuffer() {
			return &buffer[outputBufferStart];
		}

		void writeMessageLength() {
			add_header(info.length);
		}

		void addCryptoHeader(bool addChecksum) {
			if (addChecksum) {
				add_header(adlerChecksum(&buffer[outputBufferStart], info.length));
			}

			writeMessageLength();
		}

		void append(const NetworkMessage& msg) {
			auto msgLen = msg.getLength();
			std::memcpy(buffer.data() + info.position, msg.getBuffer() + 8, msgLen);
			info.length += msgLen;
			info.position += msgLen;
		}

		void append(const OutputMessage_ptr& msg) {
			auto msgLen = msg->getLength();
			std::memcpy(buffer.data() + info.position, msg->getBuffer() + 8, msgLen);
			info.length += msgLen;
			info.position += msgLen;
		}

	private:
		template <typename T>
		void add_header(T add) {
			assert(outputBufferStart >= sizeof(T));
			outputBufferStart -= sizeof(T);
			std::memcpy(buffer.data() + outputBufferStart, &add, sizeof(T));
			//added header size to the message size
			info.length += sizeof(T);
		}

		MsgSize_t outputBufferStart = INITIAL_BUFFER_POSITION;
};

namespace net {

	OutputMessage_ptr make_output_message();
	void insert_protocol_to_autosend(const Protocol_ptr& protocol);
	void remove_protocol_from_autosend(const Protocol_ptr& protocol);

} // namespace net

#endif // FS_OUTPUTMESSAGE_H