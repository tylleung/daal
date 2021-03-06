/* file: batch_normalization_layer_backward_batch_container.h */
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
//  Implementation of backward batch normalization layer container.
//--
*/

#ifndef __BATCH_NORMALIZATION_BACKWARD_BATCH_CONTAINER_H__
#define __BATCH_NORMALIZATION_BACKWARD_BATCH_CONTAINER_H__

#include "neural_networks/layers/batch_normalization/batch_normalization_layer.h"
#include "batch_normalization_layer_backward_kernel.h"

namespace daal
{
namespace algorithms
{
namespace neural_networks
{
namespace layers
{
namespace batch_normalization
{
namespace backward
{
namespace interface1
{
template<typename algorithmFPType, Method method, CpuType cpu>
BatchContainer<algorithmFPType, method, cpu>::BatchContainer(daal::services::Environment::env *daalEnv)
{
    __DAAL_INITIALIZE_KERNELS(internal::BatchNormalizationKernel, algorithmFPType, method);
}

template<typename algorithmFPType, Method method, CpuType cpu>
BatchContainer<algorithmFPType, method, cpu>::~BatchContainer()
{
    __DAAL_DEINITIALIZE_KERNELS();
}

template<typename algorithmFPType, Method method, CpuType cpu>
services::Status BatchContainer<algorithmFPType, method, cpu>::compute()
{
    batch_normalization::backward::Input *input = static_cast<batch_normalization::backward::Input *>(_in);
    batch_normalization::backward::Result *result = static_cast<batch_normalization::backward::Result *>(_res);

    batch_normalization::Parameter *parameter = static_cast<batch_normalization::Parameter *>(_par);
    daal::services::Environment::env &env = *_env;

    Tensor *weightsTensor       = input->get(auxWeights).get();
    Tensor *stDevTensor         = input->get(auxStandardDeviation).get();
    Tensor *inputGradientTensor = input->get(layers::backward::inputGradient).get();
    Tensor *dataTensor          = input->get(auxData).get();
    Tensor *meanTensor          = input->get(auxMean).get();
    Tensor *gradientTensor      = result->get(layers::backward::gradient).get();
    Tensor *weightsDerTensor    = result->get(layers::backward::weightDerivatives).get();
    Tensor *biasesDerTensor     = result->get(layers::backward::biasDerivatives).get();

    __DAAL_CALL_KERNEL(env, internal::BatchNormalizationKernel, __DAAL_KERNEL_ARGUMENTS(algorithmFPType, method),   \
                       compute, *gradientTensor, *weightsTensor, *stDevTensor, *inputGradientTensor, *dataTensor, *meanTensor,
                       *weightsDerTensor, *biasesDerTensor, *parameter);
}
} // namespace interface1
} // namespace backward

} // namespace batch_normalization
} // namespace layers
} // namespace neural_networks
} // namespace algorithms
} // namespace daal

#endif
