// Copyright (c) 2016-2021 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef GOTHAM_DEPLOYMENTINFO_H
#define GOTHAM_DEPLOYMENTINFO_H

#include <consensus/params.h>

#include <array>
#include <optional>
#include <string>

struct VBDeploymentInfo {
    /** Deployment name */
    const char *name;
    /** Whether GBT clients can safely ignore this rule in simplified usage */
    bool gbt_optional_rule;
};

extern const std::array<VBDeploymentInfo,Consensus::MAX_VERSION_BITS_DEPLOYMENTS> VersionBitsDeploymentInfo;

std::string DeploymentName(Consensus::BuriedDeployment dep);

inline std::string DeploymentName(Consensus::DeploymentPos pos)
{
    assert(Consensus::ValidDeployment(pos));
    return VersionBitsDeploymentInfo[pos].name;
}

std::optional<Consensus::BuriedDeployment> GetBuriedDeployment(const std::string_view deployment_name);

#endif // GOTHAM_DEPLOYMENTINFO_H
