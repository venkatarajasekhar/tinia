/* Copyright STIFTELSEN SINTEF 2012
 * 
 * This file is part of the Tinia Framework.
 * 
 * The Tinia Framework is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * The Tinia Framework is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with the Tinia Framework.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef POLICY_POLICYLOCK_HPP
#define POLICY_POLICYLOCK_HPP
#include "boost/thread.hpp"
#include "tinia/policy/Policy.hpp"
#include <memory>

namespace tinia {
namespace policy {
class PolicyLock
{
public:
   PolicyLock(std::shared_ptr<Policy>& policy);
   ~PolicyLock();


private:
   Policy::scoped_lock m_scoped_lock;
   std::shared_ptr<Policy> m_policy;
};

} // namespace policy
} // namespace tinia
#endif // POLICY_POLICYLOCK_HPP