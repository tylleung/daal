/* file: InitDistributedStep4LocalPlusPlusInput.java */
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

/**
 * @ingroup kmeans_init_distributed
 * @{
 */
package com.intel.daal.algorithms.kmeans.init;

import com.intel.daal.algorithms.ComputeMode;
import com.intel.daal.algorithms.Precision;
import com.intel.daal.services.DaalContext;
import com.intel.daal.data_management.data.NumericTable;
import com.intel.daal.data_management.data.DataCollection;
import com.intel.daal.data_management.data.Factory;

/**
 * <a name="DAAL-CLASS-ALGORITHMS__KMEANS__INIT__INITDISTRIBUTEDSTEP4LOCALPLUSPLUSINPUT"></a>
 * @brief Input objects for computing initial centroids for the K-Means algorithm.
 *        The class represents input objects for computing initial centroids used
 *        with plusPlus and parallelPlus methods only on the 4th step on a local node.
 */
public final class InitDistributedStep4LocalPlusPlusInput extends com.intel.daal.algorithms.kmeans.init.InitInput {
    /** @private */
    static {
        System.loadLibrary("JavaAPI");
    }

    public InitDistributedStep4LocalPlusPlusInput(DaalContext context, long cObject) {
        super(context, cObject);
    }

    /**
     * Sets an input object for computing initial centroids for the K-Means algorithm
     * @param id   Identifier of the input object
     * @param val  Value of the input object     */
    public void set(InitDistributedStep4LocalPlusPlusInputId id, NumericTable val) {
        if (id == InitDistributedStep4LocalPlusPlusInputId.inputOfStep4FromStep3) {
            cSetTable(cObject, id.getValue(), val.getCObject());
        } else {
            throw new IllegalArgumentException("id unsupported");
        }
    }

    /**
     * Returns an input object for computing initial centroids for the K-Means algorithm
     * @param id Identifier of the input object
     * @return   Input object that corresponds to the given identifier
     */
    public NumericTable get(InitDistributedStep4LocalPlusPlusInputId id) {
        if (id == InitDistributedStep4LocalPlusPlusInputId.inputOfStep4FromStep3) {
            return (NumericTable)Factory.instance().createObject(getContext(), cGetTable(cObject, id.getValue()));
        } else {
            throw new IllegalArgumentException("id unsupported");
        }
    }

    /**
    * Sets an input object for computing initial centroids for the K-Means algorithm
    * @param id   Identifier of the input object
    * @param val  Object that corresponds to the given identifier
    */
    public void set(InitDistributedLocalPlusPlusInputDataId id, DataCollection val) {
        if (id == InitDistributedLocalPlusPlusInputDataId.internalInput) {
            cSetDataCollection(cObject, id.getValue(), val.getCObject());
        } else {
            throw new IllegalArgumentException("id unsupported");
        }
    }

    /**
    * Returns an input object for computing initial centroids for the K-Means algorithm
    * @param id Identifier of the input object
    * @return   Input object that corresponds to the given identifier
    */
    public DataCollection get(InitDistributedLocalPlusPlusInputDataId id) {
        if (id != InitDistributedLocalPlusPlusInputDataId.internalInput) {
            throw new IllegalArgumentException("id unsupported");
        }
        return new DataCollection(getContext(), cGetDataCollection(cObject, id.getValue()));
    }

    private native void cSetTable(long inputAddr, int id, long ntAddr);
    private native long cGetTable(long inputAddr, int id);

    private native void cSetDataCollection(long inputAddr, int id, long ntAddr);
    private native long cGetDataCollection(long inputAddr, int id);

}
/** @} */
