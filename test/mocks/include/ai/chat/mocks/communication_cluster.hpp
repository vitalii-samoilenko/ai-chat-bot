#ifndef AI_CHAT_MOCKS_COMMUNICATION_CLUSTER_HPP
#define AI_CHAT_MOCKS_COMMUNICATION_CLUSTER_HPP

#include "gmock/gmock.h"

#include "ai/chat/model.hpp"

using namespace ::testing;

namespace ai {
namespace chat {
namespace mocks {

class CommunicationCluster {
private:


public:
	CommunicationCluster(
	) = default;
	CommunicationCluster(
		CommunicationCluster const &
	) = delete;
	CommunicationCluster(
		CommunicationCluster &&
	) = delete;

	~CommunicationCluster(
	) = default;

	CommunicationCluster
	&operator=(
		CommunicationCluster const &
	) = delete;
	CommunicationCluster
	&operator=(
		CommunicationCluster &&
	) = delete;

	MOCK_METHOD(
		void,
		push, (
			string_t,
			string_t,
			string_t,
			tags_t
		),
		(const)
	);
	MOCK_METHOD(
		void,
		notify, (
			string_t,
			string_t,
			timepoint_t,
			string_t,
			tags_t
		),
		(const)
	);
};

} // mocks
} // chat
} // ai

#endif
