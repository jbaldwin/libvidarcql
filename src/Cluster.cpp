#include "priam/Cluster.h"

#include <sstream>

namespace priam {

auto Cluster::make() -> std::unique_ptr<Cluster>
{
    return std::unique_ptr<Cluster>(new Cluster());
}

auto Cluster::AddHost(
    std::string host) -> Cluster&
{
    m_hosts.emplace_back(std::move(host));
    return *this;
}

auto Cluster::SetPort(
    uint16_t port) -> Cluster&
{
    if (cass_cluster_set_port(m_cass_cluster_ptr.get(), port) != CASS_OK) {
        throw std::runtime_error("Client: Failed to initialize port: " + std::to_string(port));
    }
    return *this;
}

auto Cluster::SetUsernamePassword(
    std::string_view username,
    std::string_view password) -> Cluster&
{
    cass_cluster_set_credentials_n(
        m_cass_cluster_ptr.get(),
        username.data(),
        username.length(),
        password.data(),
        password.length());
    return *this;
}

auto Cluster::SetRoundRobinLoadBalancing() -> bool
{
    if (m_cass_cluster_ptr != nullptr) {
        cass_cluster_set_load_balance_round_robin(m_cass_cluster_ptr.get());
    }
    return false;
}

auto Cluster::SetDatacenterAwareLoadBalancing(
    std::string_view local_dc,
    bool allow_remote_dcs_for_local_consistency_level,
    uint64_t used_hosts_per_remote_dc) -> bool
{
    if (m_cass_cluster_ptr != nullptr) {
        CassError error = cass_cluster_set_load_balance_dc_aware_n(
            m_cass_cluster_ptr.get(),
            local_dc.data(),
            local_dc.size(),
            static_cast<unsigned>(used_hosts_per_remote_dc),
            static_cast<cass_bool_t>(allow_remote_dcs_for_local_consistency_level));
        return (error == CassError::CASS_OK);
    }
    return false;
}

auto Cluster::SetTokenAwareRouting(
    bool enabled) -> bool
{
    if (m_cass_cluster_ptr != nullptr) {
        cass_cluster_set_token_aware_routing(m_cass_cluster_ptr.get(), static_cast<cass_bool_t>(enabled));
        return true;
    }
    return false;
}

auto Cluster::SetLatencyAwareRouting(
    bool enabled,
    double exclusion_threshold,
    std::chrono::milliseconds scale,
    std::chrono::milliseconds retry_period,
    std::chrono::milliseconds update_rate,
    uint64_t min_measured) -> bool
{
    if (m_cass_cluster_ptr != nullptr) {
        cass_cluster_set_latency_aware_routing(m_cass_cluster_ptr.get(), static_cast<cass_bool_t>(enabled));
        if (enabled) {
            cass_cluster_set_latency_aware_routing_settings(
                m_cass_cluster_ptr.get(),
                exclusion_threshold,
                static_cast<cass_uint64_t>(scale.count()),
                static_cast<cass_uint64_t>(retry_period.count()),
                static_cast<cass_uint64_t>(update_rate.count()),
                min_measured);
        }
        return true;
    }
    return false;
}

auto Cluster::SetHeartbeatInterval(
    std::chrono::seconds interval,
    std::chrono::seconds idle_timeout) -> bool
{
    if (m_cass_cluster_ptr != nullptr) {
        cass_cluster_set_connection_heartbeat_interval(m_cass_cluster_ptr.get(), static_cast<unsigned>(interval.count()));
        cass_cluster_set_connection_idle_timeout(m_cass_cluster_ptr.get(), static_cast<unsigned>(idle_timeout.count()));
        return true;
    }
    return false;
}

Cluster::Cluster()
    : m_cass_cluster_ptr(cass_cluster_new())
{
    if (m_cass_cluster_ptr == nullptr) {
        throw std::runtime_error("Client: Failed to initialize cassandra cluster.");
    }
}

static auto hosts_to_csv(
    const std::vector<std::string>& hosts) -> std::string
{
    std::stringstream ss{};

    std::size_t idx{0};
    for (auto& host : hosts) {
        ++idx;
        ss << host;
        if (idx < hosts.size()) {
            ss << ",";
        }
    }

    return ss.str();
}

auto Cluster::setBootstrapHosts() -> void
{
    auto contact_hosts = hosts_to_csv(m_hosts);

    if (cass_cluster_set_contact_points_n(m_cass_cluster_ptr.get(), contact_hosts.c_str(), contact_hosts.length()) != CASS_OK) {
        throw std::runtime_error("Client: Failed to initialize bootstrap contact hosts: " + contact_hosts);
    }

    // If whitelist hosts have been set also set that now.
    if (!m_whitelist_hosts.empty()) {
        auto whitelist_hosts = hosts_to_csv(m_whitelist_hosts);
        cass_cluster_set_whitelist_filtering_n(m_cass_cluster_ptr.get(), whitelist_hosts.c_str(), whitelist_hosts.length());
    }
}

} // namespace priam
