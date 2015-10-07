#ifndef AMCCVDECISIONTREE_H
#define AMCCVDECISIONTREE_H

//#include <opencv2/core/core.hpp>
//#include <opencv2/ml/ml.hpp>
#include <boost/smart_ptr.hpp>

#include "amc.h"

#include "amcclassifier.h"

class AmcCvDecisionTree : public AmcClassifier<double, AMC::ModType>
{
public:
    AmcCvDecisionTree();

    AMC::ModType classify(const std::vector<double> &predictData);
    void train(const std::vector<std::vector<double> > &trainData, const std::vector<AMC::ModType> &responses);

    void save(const std::string & fileName);

private:
//    boost::scoped_ptr<cv::CvDTree> _tree;
};


#endif // AMCCVDECISIONTREE_H
