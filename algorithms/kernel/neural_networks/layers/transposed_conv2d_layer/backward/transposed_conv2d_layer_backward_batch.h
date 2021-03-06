/* file: transposed_conv2d_layer_backward_batch.h */
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
//  Implementation of transposed convolution2d calculation algorithm and types methods.
//--
*/

#include "transposed_conv2d_layer_backward_types.h"
#include "transposed_conv2d_layer_types.h"

namespace daal
{
namespace algorithms
{
namespace neural_networks
{
namespace layers
{
namespace transposed_conv2d
{
namespace backward
{
namespace interface1
{
/**
 * Allocates memory to store the result of backward 2D transposed convolution layer
 * \param[in] input     Object containing the input data
 * \param[in] parameter %Parameter of backward 2D transposed convolution layer
 * \param[in] method    Computation method
 *
 * \return Status of computations
 */
template <typename algorithmFPType>
DAAL_EXPORT services::Status Result::allocate(const daal::algorithms::Input *input, const daal::algorithms::Parameter *parameter, const int method)
{
    using daal::data_management::Tensor;
    using daal::data_management::TensorPtr;
    using daal::data_management::HomogenTensor;

    const Input *in = static_cast<const Input *>(input);
    const Parameter *param =  static_cast<const Parameter * >(parameter);

    TensorPtr auxDataTable = in->get(auxData);
    TensorPtr wTable       = in->get(auxWeights);

    services::Collection<size_t> bDims;
    bDims.push_back(param->nKernels);

    DAAL_CHECK(auxDataTable && wTable, services::ErrorNullInputNumericTable);
    services::Status s;
    if (param->propagateGradient && !get(layers::backward::gradient))
    {
        set(layers::backward::gradient, HomogenTensor<algorithmFPType>::create(auxDataTable->getDimensions(), Tensor::doAllocate, &s));
    }
    if (!get(layers::backward::weightDerivatives))
    {
        set(layers::backward::weightDerivatives, HomogenTensor<algorithmFPType>::create(wTable->getDimensions(), Tensor::doAllocate, &s));
    }
    if (!get(layers::backward::biasDerivatives))
    {
        set(layers::backward::biasDerivatives, HomogenTensor<algorithmFPType>::create(bDims, Tensor::doAllocate, &s));
    }
    return s;
}

}// namespace interface1
}// namespace forward
}// namespace transposed_conv2d
}// namespace layers
}// namespace neural_networks
}// namespace algorithms
}// namespace daal
