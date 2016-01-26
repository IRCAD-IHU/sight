/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOTOOLS_SMATRIXTLSELECTOR_HPP__
#define __VIDEOTOOLS_SMATRIXTLSELECTOR_HPP__

#include "videoTools/config.hpp"

#include <arServices/ISynchronizer.hpp>
#include <fwCore/base.hpp>
#include <fwCore/HiResClock.hpp>
#include <fwData/Composite.hpp>
#include <fwServices/IController.hpp>
#include <fwThread/Timer.hpp>

namespace extData
{
class FrameTL;
class MatrixTL;
}

namespace fwData
{
class Image;
class TransformationMatrix3D;
}

namespace videoTools
{
/**
 * @brief   SMatrixTLSelector service synchronizes video frame and tracking matrixes.
 * @class   SMatrixTLSelector
 */
class VIDEOTOOLS_CLASS_API SMatrixTLSelector : public ::arServices::ISynchronizer
{

public:

    fwCoreServiceClassDefinitionsMacro((SMatrixTLSelector)(fwServices::IController));

    /**
     * @brief Constructor.
     */
    VIDEOTOOLS_API SMatrixTLSelector() throw ();

    /**
     * @brief Destructor.
     */
    virtual ~SMatrixTLSelector() throw ()
    {
    }

    class MatrixCfgType
    {
    public:
        size_t m_index;

        std::string m_matrixID;
        SPTR(::fwData::TransformationMatrix3D) m_matrix;

        std::vector<std::string> m_timelineKeyVect;
        /// registers matrix timeline with its id in composite
        std::map<std::string, SPTR(::extData::MatrixTL)> m_timelineObjectMap;
    };

    typedef std::vector< MatrixCfgType > MatrixCfgVectorType;

    typedef std::map<std::string, std::string> FrameKeysType;

    typedef std::map<std::string, SPTR(::extData::FrameTL)> FrameTLKeyType;
    typedef std::map<std::string, SPTR(::fwData::Image)> ImageKeyType;

protected:
    /**
     * @brief This method is used to configure the service.
     *
     * @code{.xml}
        <service uid="synchronizer" impl="::videoTools::SMatrixTLSelector" autoConnect="no" worker="videoWorker">
            <frames>
                <frame from="frameTL" to="image" />
            </frames>
            <matrices>
                <matrix index="0" to="transform" />
                    <timeline from="matrixTL1"/>
                    <timeline from="matrixTL2"/>
                    <timeline from="matrixTL3"/>
                </matrix>
                <matrix index="0" to="transform2" />
                    <timeline from="matrixTL11"/>
                    <timeline from="matrixTL114"/>
                </matrix>
                <matrix index="0" to="transform4" />
                    <timeline from="matrixTL11"/>
                </matrix>
            </matrices>
            <framerate>60</framerate>
        </service>

       @endcode
     */
    VIDEOTOOLS_API void configuring() throw (fwTools::Failed);

    /// This method is used to initialize the service.
    VIDEOTOOLS_API void starting() throw (fwTools::Failed);

    /// Does nothing.
    VIDEOTOOLS_API void stopping() throw (fwTools::Failed);

    /// Does nothing.
    void updating() throw (fwTools::Failed)
    {
    }

private:

    /// Synchronize
    void synchronize();

    void updateFrames(::fwCore::HiResClock::HiResClockType timestamp);

    ::fwCore::HiResClock::HiResClockType m_lastTimestamp;
    ::fwCore::HiResClock::HiResClockType m_tolerance; ///< Tolerance to take into account matrix

    bool m_imagesInitialized;


    ///< registers matrices with associated timeline key
    MatrixCfgVectorType m_matrixCfgVector;

    /// registers FrameTL keys to synchronize and the associated fwData::Image keys
    FrameKeysType m_frameKeys;

    ///< registers frame timeline with its id in composite
    FrameTLKeyType m_frameTLs;

    ///< registers frames with associated timeline key
    ImageKeyType m_images;

    ///< Time step used for the update
    unsigned int m_timeStep;

    ///< Timer used for the update
    ::fwThread::Timer::sptr m_timer;
};

} //namespace videoTools
#endif  // __VIDEOTOOLS_SMATRIXTLSELECTOR_HPP__
