/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "scene2D/config.hpp"

#include <data/Histogram.hpp>
#include <data/Point.hpp>

#include <fwRenderQt/IAdaptor.hpp>

namespace scene2D
{

namespace adaptor
{

/**
 * @brief IAdaptor implementation for histogram data.
 *
 * @section XML XML Configuration
 *
   @code{.xml}
   <service uid="histogram" type="::scene2D::adaptor::SCurvedHistogram" autoConnect="yes">
       <in key="histogram" uid="..." />
       <inout key="point" uid="..." optional="yes" />
       <config xAxis="xAxis" yAxis="yAxis" borderColor="lightGray" innerColor="gray"
               opacity="0.25" zValue="6" borderWidth="2.0" />
   </service>
   @endcode
 *
 * @subsection In In
 * - \b histogram [data::Histogram]: histogram to display.
 *
 * @subsection In-Out In-Out
 * - \b point [data::Point]: histogram point, used to show information at the current histogram index pointed by the
 * mouse.
 *
 * @subsection Configuration Configuration:
 * - \b config (mandatory): contains the adaptor configuration
 *    - \b innerColor (optional, default black): the background color of the histogram
 *    - \b borderColor (optional, default black): the color of the histogram border
 *    - \b borderWidth (optional, default 1.75): the width of the histogram border
 *    - \b xAxis (optional): x axis associated to the adaptor
 *    - \b yAxis (optional): y axis associated to the adaptor
 *    - \b zValue (optional, default=0): z value of the layer
 *    - \b opacity (optional, default=1.0): adaptor opacity (float)
 *    - \b color (optional, default black): color of the axis
 */
class SCENE2D_CLASS_API SCurvedHistogram : public ::fwRenderQt::IAdaptor
{
public:
    fwCoreServiceMacro(SCurvedHistogram,  ::fwRenderQt::IAdaptor)

    typedef ::fwRenderQt::IAdaptor::Point2DType Point;
    typedef std::vector< Point > Points;

    SCENE2D_API SCurvedHistogram() noexcept;
    SCENE2D_API virtual ~SCurvedHistogram() noexcept;

protected:
    SCENE2D_API void configuring() override;
    SCENE2D_API void starting() override;
    SCENE2D_API void updating() override;
    SCENE2D_API void stopping() override;

    SCENE2D_API void processInteraction( ::fwRenderQtdata::Event& _event ) override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Histogram::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    SCENE2D_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Ratio used for vertical scaling (default value: 1.1)
    static const float SCALE;

    /// The number of points between to points of the final Bezier curve to compute
    static const float NB_POINTS_BEZIER;

private:

    Points getControlPoints(const data::Histogram::csptr& _histogram ) const;

    Points getBSplinePoints( const Points& _controlPoints ) const;

    Points getResampledBSplinePoints( const Points& _bSplinePoints ) const;

    void buildBSplineFromPoints( Points& _bSplinePoints );

    void computePointToPathLengthMapFromBSplinePoints( Points& _bSplinePoints );

    /// Update the value of m_ordinateValueUID according to the value pointed by mouse cursor.
    void updateCurrentPoint(const ::fwRenderQtdata::Event& _event, const data::Point::sptr& point );

    /// Build and add a part of histogram's border, according to the given path.
    void addBorderItem( const QPainterPath& _path );

    /// Build and add a part of the histogram, according to the given path.
    void addInnerItem( const QPainterPath& _path );

    Points linearInterpolation( const Point _p1, const Point _p2 );

    Points cosinusInterpolation( const Point _p0, const Point _p1 );

    Points quadraticInterpolation( const Point _p0, const Point _p1, const Point _p2 );

    Points cubicInterpolation(const Point _p0, const Point _p1, const Point _p2, const Point _p3 );

    // Map the absciss of the points to the corresponding length within the path.
    std::map<double, double> m_positionsToPathLength;

    /// Color used for graphic histogram's border color
    QPen m_borderColor;

    // Color used for graphic histogram's inner color. If no value is supplied in the configuration,
    // item's inner will be transparent
    QPen m_innerColor;

    /// Inner color.
    QBrush m_brush;

    // A Qt painter that is used to build a curve representing the shape of the histogram.
    // Note that the histogram won't be built thanks to this path because of a lack of performance.
    // In other words, this painter path won't be used to build a graphic item that will be added to
    // a Qt graphics scene. The graphic representation of the histogram will composed of multiple items
    // build thanks to painters like this one, because it improves significantly the rendering performance.
    // This painter path will be used to provide information about the histogram: for instance, it will help
    // to retrieve the coordinates of path's points.
    QPainterPath* m_painterPath;

    /// Width of histram's border
    float m_borderWidth;

    /// Current vertical scaling ratio
    float m_scale;

    // Graphics items contained into m_items are also added to this group item,
    // which is then added to the scene.
    // (This is the only graphic item which has to be added into the scene).
    QGraphicsItemGroup* m_layer;
};

}   // namespace adaptor

}   // namespace scene2D
