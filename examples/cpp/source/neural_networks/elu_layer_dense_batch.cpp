/* file: elu_layer_dense_batch.cpp */
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
!  Content:
!    C++ example of forward and backward Exponential Linear Unit (ELU) layer usage
!
!******************************************************************************/

/**
 * <a name="DAAL-EXAMPLE-CPP-ELU_LAYER_BATCH"></a>
 * \example elu_layer_dense_batch.cpp
 */

#include "daal.h"
#include "service.h"

using namespace std;
using namespace daal;
using namespace daal::algorithms;
using namespace daal::algorithms::neural_networks::layers;
using namespace daal::data_management;
using namespace daal::services;

/* Input data set parameters */
string datasetName = "../data/batch/layer.csv";

int main()
{
    /* Read datasetFileName from a file and create a tensor to store input data */
    TensorPtr tensorData = readTensorFromCSV(datasetName);

    /* Create an algorithm to compute forward ELU layer results using default method */
    elu::forward::Batch<> eluLayerForward;

    /* Set input objects for the forward ELU layer */
    eluLayerForward.input.set(forward::data, tensorData);

    /* Compute forward ELU layer results */
    eluLayerForward.compute();

    // /* Print the results of the forward ELU layer */
    elu::forward::ResultPtr forwardResult = eluLayerForward.getResult();
    printTensor(forwardResult->get(forward::value), "Forward ELU layer result (first 5 rows):", 5);

    // /* Get the size of forward ELU layer output */
    const Collection<size_t> &gDims = forwardResult->get(forward::value)->getDimensions();
    TensorPtr tensorDataBack = TensorPtr(new HomogenTensor<>(gDims, Tensor::doAllocate, 1.0f));

    // /* Create an algorithm to compute backward ELU layer results using default method */
    elu::backward::Batch<> eluLayerBackward;

    // /* Set input objects for the backward ELU layer */
    eluLayerBackward.input.set(backward::inputGradient, tensorDataBack);
    eluLayerBackward.input.set(backward::inputFromForward, forwardResult->get(forward::resultForBackward));

    // /* Compute backward ELU layer results */
    eluLayerBackward.compute();

    // /* Print the results of the backward ELU layer */
    backward::ResultPtr backwardResult = eluLayerBackward.getResult();
    printTensor(backwardResult->get(backward::gradient), "Backward ELU layer result (first 5 rows):", 5);

    return 0;
}
