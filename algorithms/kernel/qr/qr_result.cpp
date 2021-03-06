/* file: qr_result.cpp */
/*******************************************************************************
* Copyright 2014-2018 Intel Corporation
* All Rights Reserved.
*
* If this  software was obtained  under the  Intel Simplified  Software License,
* the following terms apply:
*
* The source code,  information  and material  ("Material") contained  herein is
* owned by Intel Corporation or its  suppliers or licensors,  and  title to such
* Material remains with Intel  Corporation or its  suppliers or  licensors.  The
* Material  contains  proprietary  information  of  Intel or  its suppliers  and
* licensors.  The Material is protected by  worldwide copyright  laws and treaty
* provisions.  No part  of  the  Material   may  be  used,  copied,  reproduced,
* modified, published,  uploaded, posted, transmitted,  distributed or disclosed
* in any way without Intel's prior express written permission.  No license under
* any patent,  copyright or other  intellectual property rights  in the Material
* is granted to  or  conferred  upon  you,  either   expressly,  by implication,
* inducement,  estoppel  or  otherwise.  Any  license   under such  intellectual
* property rights must be express and approved by Intel in writing.
*
* Unless otherwise agreed by Intel in writing,  you may not remove or alter this
* notice or  any  other  notice   embedded  in  Materials  by  Intel  or Intel's
* suppliers or licensors in any way.
*
*
* If this  software  was obtained  under the  Apache License,  Version  2.0 (the
* "License"), the following terms apply:
*
* You may  not use this  file except  in compliance  with  the License.  You may
* obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
*
*
* Unless  required  by   applicable  law  or  agreed  to  in  writing,  software
* distributed under the License  is distributed  on an  "AS IS"  BASIS,  WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
* See the   License  for the   specific  language   governing   permissions  and
* limitations under the License.
*******************************************************************************/

/*
//++
//  Implementation of qr classes.
//--
*/

#include "algorithms/qr/qr_types.h"
#include "serialization_utils.h"
#include "daal_strings.h"

using namespace daal::data_management;
using namespace daal::services;

namespace daal
{
namespace algorithms
{
namespace qr
{
namespace interface1
{
__DAAL_REGISTER_SERIALIZATION_CLASS(Result, SERIALIZATION_QR_RESULT_ID);

/** Default constructor */
Result::Result() : daal::algorithms::Result(lastResultId + 1) {}

/**
 * Returns the result of the QR decomposition algorithm
 * \param[in] id    Identifier of the result
 * \return          Result that corresponds to the given identifier
 */
NumericTablePtr Result::get(ResultId id) const
{
    return staticPointerCast<NumericTable, SerializationIface>(Argument::get(id));
}

/**
 * Sets an input object for the QR decomposition algorithm
 * \param[in] id    Identifier of the result
 * \param[in] value Pointer to the result
 */
void Result::set(ResultId id, const NumericTablePtr &value)
{
    Argument::set(id, value);
}

/**
 * Checks final results of the algorithm
 * \param[in] input  Pointer to input objects
 * \param[in] par    Pointer to parameters
 * \param[in] method Computation method
 */
Status Result::check(const daal::algorithms::Input *input, const daal::algorithms::Parameter *par, int method) const
{
    const Input *algInput = static_cast<const Input *>(input);
    size_t nVectors = algInput->get(data)->getNumberOfRows();
    size_t nFeatures = algInput->get(data)->getNumberOfColumns();
    int unexpectedLayouts = (int)packed_mask;

    Status s = checkNumericTable(get(matrixQ).get(), matrixQStr(), unexpectedLayouts, 0, nFeatures, nVectors);
    if(!s) { return s; }

    s |= checkNumericTable(get(matrixR).get(), matrixRStr(), unexpectedLayouts, 0, nFeatures, nFeatures);
    return s;
}

/**
 * Checks the result parameter of the QR algorithm
 * \param[in] pres    Partial result of the algorithm
 * \param[in] par     %Parameter of the algorithm
 * \param[in] method  Computation method
 */
Status Result::check(const daal::algorithms::PartialResult *pres, const daal::algorithms::Parameter *par, int method) const
{
    const OnlinePartialResult  *algPartRes = static_cast<const OnlinePartialResult *>(pres);
    int unexpectedLayouts = (int)packed_mask;
    size_t nVectors = algPartRes->getNumberOfRows();
    size_t nFeatures = algPartRes->getNumberOfColumns();

    Status s = checkNumericTable(get(matrixQ).get(), matrixQStr(), unexpectedLayouts, 0, nFeatures, nVectors);
    if(!s) { return s; }

    s |= checkNumericTable(get(matrixR).get(), matrixRStr(), unexpectedLayouts, 0, nFeatures, nFeatures);
    return s;
}

} // namespace interface1
} // namespace qr
} // namespace algorithm
} // namespace daal
