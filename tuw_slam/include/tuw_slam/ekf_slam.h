#ifndef EKF_SLAM_H
#define EKF_SLAM_H

#include <tuw_geometry/tuw_geometry.h>
#include "tuw_slam/slam_technique.h"
#include "tuw_slam/EKFSLAMConfig.h"

namespace tuw {

class EKFSLAM;
typedef std::shared_ptr< EKFSLAM > EKFSLAMPtr;
typedef std::shared_ptr< EKFSLAM const> EKFSLAMConstPtr;

/**
 * extended kalman filter slam
 */
class EKFSLAM : public SLAMTechnique {
public:
    /**
    * Constructor
    **/
    EKFSLAM();
    /**
     * starts the SLAM cycle and predicts the robot and landmark poses at the timestamp encoded into the measurement
     * @param yt TODO
     * @param C_Yt TODO
     * @param ut current control command
     * @param zt measurment with a timestamp
     **/
    void cycle ( std::vector<Pose2D> &yt, cv::Mat_<double> &C_Yt, const Command &ut, const MeasurementConstPtr &zt );
    /**
     * virtual function to set the config parameters
     * @param config of type tuw_slam::EKFSLAMConfig*
     **/
    void setConfig ( const void *config );
private:
    enum DataAssociationMode {
        ID = 0,
        NNSF_LOCAL = 1,
        NNSF_GLOBAL = 2,
    };
    enum UpdateMode {
        None = 0,
        Single = 1,
        Combined = 2,
    };
    /**
    * struct to represent correspondeces and their data
    **/
    struct CorrData {
        std::pair<size_t, size_t> ij;           /// i >= 0, j > 0...landmark j corressponds to measurement i (else no correspondence)
        cv::Vec<double, 3> v;                   /// difference of obtained and predicted measurement
        cv::Matx<double, 3, 3> dx;              /// x deviation in H_ij = (dx 0 .. 0 dm 0 .. 0)
        cv::Matx<double, 3, 3> dm;              /// m deviation in H_ij = (dx 0 .. 0 dm 0 .. 0)
        cv::Matx<double, 3, 3> S_inv;           /// inverse ??? matrix (TODO)
    };
    typedef std::shared_ptr< CorrData > CorrDataPtr;
    typedef std::shared_ptr< CorrData const > CorrDataConstPtr;
    /**
     * initializes the filter
     **/
    void init();
    /**
     * predicting robots pose
     * @param ut command at time t
     **/
    void prediction ( const Command &ut );
    /**
     * data association to find matching between measurements and landmarks
     * @param zt measurement at time t
     **/
    void data_association ( const MeasurementMarkerConstPtr &zt );
    /**
     * TODO
     **/
    void NNSF_local ( const MeasurementMarkerConstPtr &zt, const double gamma );
    /**
     * TODO
     **/
    void NNSF_global ( const MeasurementMarkerConstPtr &zt, const double gamma );
    /**
     * TODO
     **/
    void measurement ( const MeasurementMarkerConstPtr &zt, const CorrDataPtr &corr );
    /**
     * correcting robot and landmarks poses
     * @param zt measurement at time t
     **/
    void update();
    /**
     * TODO
     **/
    void update_single();
    /**
     * TODO
     **/
    void update_combined();
    /**
     * integrating new landmarks
     **/
    void integration ( const MeasurementMarkerConstPtr &zt );

    tuw_slam::EKFSLAMConfig config_;            /// parameters

    cv::Mat_<double> y;                         /// mean vector of y = (x m1 m2 ...)
    cv::Mat_<double> x;                         /// mean vector of x
    cv::Mat_<double> C_Y;                       /// covariance matrix of y = (x m1 m2 ...)
    cv::Mat_<double> C_X;                       /// covariance matrix of x

    cv::Matx<double, 2, 2> M;                   /// control noise
    cv::Matx<double, 3, 3> Q;                   /// measurement noise

    std::map<size_t, size_t> f_kj;              /// correspondences: f[k] = j <-> fiducial k corressponds to landmark j

    std::vector<CorrDataPtr> c_ij;              /// measurement to landmark correspondences
    std::vector<CorrDataPtr> c_ji;              /// landmark to measurement correspondences (ignore j = 0 since landmark numbering starts with 1)

    std::vector<size_t> z_known;                /// measurements corresponding to known landmarks
    std::vector<size_t> z_new;                  /// measurements corresponding to new landmarks
};
};

#endif // EKF_SLAM_H
