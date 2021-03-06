/* file: mse_dense_default_batch_impl.i */
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
//  Implementation of mse algorithm
//--
*/

namespace daal
{
namespace algorithms
{
namespace optimization_solver
{
namespace mse
{
namespace internal
{
/**
 *  \brief Kernel for mse objective function calculation
 */
template<typename algorithmFPType, Method method, CpuType cpu>
inline services::Status MSEKernel<algorithmFPType, method, cpu>::compute(NumericTable *dataNT, NumericTable *dependentVariablesNT, NumericTable *argumentNT,
                                                             NumericTable *valueNT, NumericTable *hessianNT, NumericTable *gradientNT, Parameter *parameter)
{
    const size_t nDataRows = dataNT->getNumberOfRows();
    if(parameter->batchIndices.get() != NULL && parameter->batchIndices->getNumberOfColumns() != nDataRows)
    {
        MSETaskSample<algorithmFPType, cpu> task(dataNT, dependentVariablesNT, argumentNT, valueNT, hessianNT, gradientNT, parameter, blockSizeDefault);
        return run(task);
    }
    MSETaskAll<algorithmFPType, cpu> task(dataNT, dependentVariablesNT, argumentNT, valueNT, hessianNT, gradientNT, parameter, blockSizeDefault);
    return run(task);
}

template<typename algorithmFPType, Method method, CpuType cpu>
services::Status MSEKernel<algorithmFPType, method, cpu>::run(MSETask<algorithmFPType, cpu>& task)
{
    algorithmFPType *argumentArray = NULL;
    Status s = task.init(argumentArray);
    if(!s)
        return s;
    algorithmFPType *dataBlock = NULL, *dependentVariablesBlock = NULL;
    algorithmFPType *value = NULL, *gradient = NULL, *hessian = NULL;
    DAAL_CHECK_STATUS(s, task.getResultValues(value, gradient, hessian));
    task.setResultValuesToZero(value, gradient, hessian);

    size_t blockSize = blockSizeDefault;
    size_t nBlocks = task.batchSize / blockSizeDefault;
    nBlocks += (nBlocks * blockSizeDefault != task.batchSize);
    if(nBlocks == 1) { blockSize = task.batchSize; }

    for(size_t block = 0; s.ok() && (block < nBlocks); block++)
    {
        if( block == nBlocks - 1 )
            blockSize = task.batchSize - block * blockSizeDefault;

        s = task.getCurrentBlock(block * blockSizeDefault, blockSize, dataBlock, dependentVariablesBlock);
        if(s)
            computeMSE(blockSize, task, dataBlock, argumentArray, dependentVariablesBlock, value, gradient, hessian);
        task.releaseCurrentBlock();
    }

    if(s)
        normalizeResults(task, value, gradient, hessian);
    task.releaseResultValues();
    return s;
}

template<typename algorithmFPType, Method method, CpuType cpu>
inline void MSEKernel<algorithmFPType, method, cpu>::computeMSE(
    size_t blockSize,
    MSETask<algorithmFPType, cpu> &task,
    algorithmFPType *data,
    algorithmFPType *argumentArray,
    algorithmFPType *dependentVariablesArray,
    algorithmFPType *value,
    algorithmFPType *gradient,
    algorithmFPType *hessian)
{
    char trans = 'T';
    algorithmFPType one = 1.0;
    algorithmFPType zero = 0.0;
    DAAL_INT n   = (DAAL_INT)blockSize;
    size_t nTheta = task.nTheta;
    DAAL_INT dim = (DAAL_INT)nTheta;
    DAAL_INT ione = 1;
    algorithmFPType theta0 = argumentArray[0];
    algorithmFPType *theta = &argumentArray[1];
    algorithmFPType *xMultTheta = task.xMultTheta.get();

    if (task.gradientFlag || task.valueFlag)
    {
        Blas<algorithmFPType, cpu>::xgemv(&trans, &dim, &n, &one, data, &dim, theta, &ione, &zero, xMultTheta, &ione);

        for(size_t i = 0; i < blockSize; i++)
        {
            xMultTheta[i] = xMultTheta[i] + theta0 - dependentVariablesArray[i];
        }
    }

    if (task.gradientFlag)
    {
        for(size_t i = 0; i < blockSize; i++)
        {
            gradient[0] += xMultTheta[i];
            for(size_t j = 0; j < nTheta; j++)
            {
                gradient[j + 1] += xMultTheta[i] * data[i * nTheta + j];
            }
        }
    }

    if (task.valueFlag)
    {
        for(size_t i = 0; i < blockSize; i++)
        {
            value[0] += xMultTheta[i] * xMultTheta[i];
        }
    }

    if (task.hessianFlag)
    {
        char uplo  = 'U';
        char notrans = 'N';
        DAAL_INT argumentSize = dim + 1;

        Blas<algorithmFPType, cpu>::xsyrk(&uplo, &notrans, &dim, &n, &one, data, &dim, &one, hessian + argumentSize + 1, &argumentSize);

        for (size_t i = 0; i < blockSize; i++)
        {
            for (size_t j = 0; j < nTheta; j++)
            {
                hessian[j + 1] += data[i * nTheta + j];
            }
        }

        for (size_t i = 0; i < argumentSize; i++)
        {
            for (size_t j = 1; j < i; j++)
            {
                hessian[j * argumentSize + i] = hessian[i * argumentSize + j];
            }
            hessian[i * argumentSize] = hessian[i];
        }
    }
}

template<typename algorithmFPType, Method method, CpuType cpu>
void MSEKernel<algorithmFPType, method, cpu>::normalizeResults(
    MSETask<algorithmFPType, cpu> &task,
    algorithmFPType *value,
    algorithmFPType *gradient,
    algorithmFPType *hessian
)
{
    size_t argumentSize = task.argumentSize;
    const algorithmFPType one = 1.0;
    algorithmFPType batchSizeInv = (algorithmFPType)one / task.batchSize;
    if (task.valueFlag)
    {
        value[0] /= (algorithmFPType)(2 * task.batchSize);
    }

    if (task.gradientFlag)
    {
        for(size_t j = 0; j < argumentSize; j++)
        {
            gradient[j] *= batchSizeInv;
        }
    }

    if (task.hessianFlag)
    {
        hessian[0] = one;
        for(size_t j = 1; j < argumentSize * argumentSize; j++)
        {
            hessian[j] *= batchSizeInv;
        }
    }
}

} // namespace daal::internal

} // namespace mse

} // namespace optimization_solver

} // namespace algorithms

} // namespace daal
