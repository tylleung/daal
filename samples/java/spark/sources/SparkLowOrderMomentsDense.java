/* file: SparkLowOrderMomentsDense.java */
//==============================================================
//
// SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
// http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
//
// Copyright 2017-2018 Intel Corporation
//
// THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
//
// =============================================================

/*
//  Content:
//      Java sample of computing low order moments in the distributed
//      processing mode
////////////////////////////////////////////////////////////////////////////////
*/

package DAAL;

import java.util.ArrayList;
import java.util.List;
import java.util.Arrays;
import java.util.Map;
import java.util.HashMap;

import org.apache.spark.api.java.*;
import org.apache.spark.api.java.function.*;
import org.apache.spark.SparkConf;

import scala.Tuple2;

import com.intel.daal.algorithms.low_order_moments.*;
import com.intel.daal.data_management.data.*;
import com.intel.daal.services.*;

public class SparkLowOrderMomentsDense {
    /* Class containing the algorithm results */
    static class MomentsResult {
        public HomogenNumericTable minimum;
        public HomogenNumericTable maximum;
        public HomogenNumericTable sum;
        public HomogenNumericTable sumSquares;
        public HomogenNumericTable sumSquaresCentered;
        public HomogenNumericTable mean;
        public HomogenNumericTable secondOrderRawMoment;
        public HomogenNumericTable variance;
        public HomogenNumericTable standardDeviation;
        public HomogenNumericTable variation;
    }

    static JavaPairRDD<Integer, PartialResult> partsRDD;

    public static MomentsResult runMoments(DaalContext context, JavaRDD<HomogenNumericTable> dataRDD) {
        JavaRDD<PartialResult> partsRDD = computestep1Local(dataRDD);

        PartialResult finalPartRes = reducePartialResults(partsRDD);

        MomentsResult result = finalizeMergeOnMasterNode(context, finalPartRes);

        return result;
    }

    private static JavaRDD<PartialResult> computestep1Local(JavaRDD<HomogenNumericTable> dataRDD) {
        return dataRDD.map(new Function<HomogenNumericTable, PartialResult>() {
            public PartialResult call(HomogenNumericTable table) {
                DaalContext context = new DaalContext();

                /* Create an algorithm to compute low order moments on local nodes */
                DistributedStep1Local momentsLocal = new DistributedStep1Local(context, Double.class, Method.defaultDense);

                /* Set the input data on local nodes */
                table.unpack(context);
                momentsLocal.input.set( InputId.data, table );

                /* Compute low order moments on local nodes */
                PartialResult pres = momentsLocal.compute();
                pres.pack();

                context.dispose();
                return pres;
            }
        });
    }

    private static PartialResult reducePartialResults(JavaRDD<PartialResult> partsRDD) {
        return partsRDD.reduce(new Function2<PartialResult, PartialResult, PartialResult>() {
            public PartialResult call(PartialResult p1, PartialResult p2) {
                DaalContext context = new DaalContext();

                /* Create an algorithm to compute new partial result from two partial results */
                DistributedStep2Master momentsMaster = new DistributedStep2Master(context, Double.class, Method.defaultDense);

                /* Set the partial results recieved from the local nodes */
                p1.unpack(context);
                p2.unpack(context);
                momentsMaster.input.add(DistributedStep2MasterInputId.partialResults, p1);
                momentsMaster.input.add(DistributedStep2MasterInputId.partialResults, p2);

                /* Compute a new partial result from two partial results */
                PartialResult pres = momentsMaster.compute();
                pres.pack();

                context.dispose();
                return pres;
            }
        });
    }

    private static MomentsResult finalizeMergeOnMasterNode(DaalContext context, PartialResult partRes) {

        /* Create an algorithm to compute low order moments on the master node */
        DistributedStep2Master momentsMaster = new DistributedStep2Master(context, Double.class, Method.defaultDense);

        /* Add partial results computed on local nodes to the algorithm on the master node */
        partRes.unpack(context);
        momentsMaster.input.add(DistributedStep2MasterInputId.partialResults, partRes);

        /* Compute low order moments on the master node */
        momentsMaster.compute();

        /* Finalize computations and retrieve the results */
        Result res = momentsMaster.finalizeCompute();

        MomentsResult result = new MomentsResult();
        result.minimum              = (HomogenNumericTable)res.get(ResultId.minimum);
        result.maximum              = (HomogenNumericTable)res.get(ResultId.maximum);
        result.sum                  = (HomogenNumericTable)res.get(ResultId.sum);
        result.sumSquares           = (HomogenNumericTable)res.get(ResultId.sumSquares);
        result.sumSquaresCentered   = (HomogenNumericTable)res.get(ResultId.sumSquaresCentered);
        result.mean                 = (HomogenNumericTable)res.get(ResultId.mean);
        result.secondOrderRawMoment = (HomogenNumericTable)res.get(ResultId.secondOrderRawMoment);
        result.variance             = (HomogenNumericTable)res.get(ResultId.variance);
        result.standardDeviation    = (HomogenNumericTable)res.get(ResultId.standardDeviation);
        result.variation            = (HomogenNumericTable)res.get(ResultId.variation);
        return result;
    }
}
