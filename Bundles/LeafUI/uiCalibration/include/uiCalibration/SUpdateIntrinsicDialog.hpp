/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UICALIBRATION_SUPDATEINTRINSICDIALOG_HPP__
#define __UICALIBRATION_SUPDATEINTRINSICDIALOG_HPP__

#include <QObject>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>

#include <boost/array.hpp>

#include "uiCalibration/config.hpp"

namespace uiCalibration
{
/**
 * @brief The SUpdateIntrinsicDialog class displays a QDialog. The user can change the calibration resolution which leads to an automatic computation of the new calibration parameters. By validating the user entry, the calibration will be updated.
 */
class UICALIBRATION_CLASS_API SUpdateIntrinsicDialog : public QDialog
{

Q_OBJECT;

public:

    /// constructor
    SUpdateIntrinsicDialog();

    /// destructor
    ~SUpdateIntrinsicDialog();

    /**
     * @brief setParameters
     * @param calibration parameters in a array< double , 12>
     */
    void setParameters(::boost::array< double, 12>& parameters);

Q_SIGNALS:

    void newCalibration(::boost::array< double, 12 >& newParameters);


private Q_SLOTS:

    /**
     * @brief onPushCompute compute the calibration with new resolution
     */
    void onPushCompute();

    /**
     * @brief onPushReset reset the original calibration
     */
    void onPushReset();

    /**
     * @brief onValidate send the signal newCalibration
     */
    void onValidate();

private:

    /**
     * @brief updateInfos display new calibration
     */
    void updateInfos();

    /**
     * @brief concatenation of resolution (0-1) , intrinsic parameters (2-5), distortions (6-10), and skew(11)
     */
    ::boost::array< double, 12 > m_calibration;
    /**
     * @brief store the original calibration (to reset)
     */
    ::boost::array< double, 12 > m_originCalibration;

    /**
     * @brief Width of the video.
     */
    QLineEdit* m_width;

    /**
     * @brief Height of the video.
     */
    QLineEdit* m_height;

    /**
     * @brief Optical center x.
     */
    QLabel* m_cx;

    /**
     * @brief Optical center y.
     */
    QLabel* m_cy;

    /**
     * @brief Field of view x.
     */
    QLabel* m_fx;

    /**
     * @brief Field of view y.
     */
    QLabel* m_fy;

    /**
     * @brief Distortion coefficient k1.
     */
    QLabel* m_k1;

    /**
     * @brief Distortion coefficient k2.
     */
    QLabel* m_k2;

    /**
     * @brief Distortion coefficient p1.
     */
    QLabel* m_p1;

    /**
     * @brief Distortion coefficient p2.
     */
    QLabel* m_p2;

    /**
     * @brief Distortion coefficient k3.
     */
    QLabel* m_k3;

    /**
     * @brief Camera skew.
     */
    QLabel* m_skew;

    /// ratio of the calibration resolution
    double m_ratio;


};

} //namespace uiCalibration

#endif // __UICALIBRATION_SUPDATEINTRINSICDIALOG_HPP__
