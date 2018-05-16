#ifndef BOOST_ASTRONOMY_COORDINATE_BASE_REPRESENTATION_HPP
#define BOOST_ASTRONOMY_COORDINATE_BASE_REPRESENTATION_HPP


#include <cmath>
#include <ostream>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/algorithms/transform.hpp>
#include <boost/geometry/algorithms/equals.hpp>
#include <boost/geometry/arithmetic/cross_product.hpp>
#include <boost/geometry/arithmetic/dot_product.hpp>
#include <boost/geometry/core/cs.hpp>
#include<boost/geometry/io/dsv/write.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/range_c.hpp>

namespace boost
{
    // structure to provide support like std::is_base_of for template base classes
    template <template <int, typename...> class Base, typename Derived>
    struct base_template
    {
        using U = typename std::remove_cv<Derived>::type;

        template <int T, typename... Args>
        static std::true_type test(Base<T, Args...>*);

        static std::false_type test(void*);

        using type = decltype(test(std::declval<U*>()));
    };

    template <template <int, typename...> class Base, typename Derived>
    using is_base_template_of = typename base_template<Base, Derived>::type;

    namespace astronomy
    {
        namespace coordinate
        {
            // abstract structure which is the base for all the representation 
            template <int DimensionCount, typename Type>
            struct base_representation
            {
            protected:
                boost::geometry::model::point<double, DimensionCount, Type> point;

            public:
                
                // cross prodct of current vector with specified vector
                template <typename ReturnType, typename Representation>
                ReturnType cross(Representation const& other) const
                {
                    /*both the coordinates/vector are first converted into cartesian coordinate system then
                    cross product of both cartesian vectors is converted into requested type and returned*/

                    /*checking types of argument and return type if they both are not
                    subclass of base_representaion then compile time erorr is generated*/
                    BOOST_STATIC_ASSERT_MSG((boost::is_base_template_of
                        <boost::astronomy::coordinate::base_representation, Representation>::value),
                        "function argument type is expected to be a representation type");
                    
                    BOOST_STATIC_ASSERT_MSG((boost::is_base_template_of
                        <boost::astronomy::coordinate::base_representation, ReturnType>::value), 
                        "return type is expected to be a representation class");

                    /*converting both coordinates/vector into cartesian system*/
                    boost::geometry::model::point<double, 3, boost::geometry::cs::cartesian> tempPoint1, tempPoint2;
                    boost::geometry::transform(this->point, tempPoint1);
                    boost::geometry::transform(other.get_point(), tempPoint2);

                    /*creating object of the specified return type and returning it */
                    return ReturnType(boost::geometry::cross_product
                        <boost::geometry::model::point<double, 3, boost::geometry::cs::cartesian>>
                        (tempPoint1, tempPoint2));
                }
                
                // dot prodct of current vector with specified vector
                template <typename Representation>
                double dot(Representation const& other) const
                {
                    /*both the coordinates/vector are first converted into cartesian coordinate system then
                    dot product of both cartesian product is converted into requested type and returned*/

                    /*checking types of argument if they both are not subclass 
                    of base_representaion then compile time erorr is generated*/
                    BOOST_STATIC_ASSERT_MSG((boost::is_base_template_of
                        <boost::astronomy::coordinate::base_representation, Representation>::value),
                        "function argument type is expected to be a representation type");

                    /*converting both coordinates/vector into cartesian system*/
                    boost::geometry::model::point<double, 3, boost::geometry::cs::cartesian> tempPoint1, tempPoint2;
                    boost::geometry::transform(this->point, tempPoint1);
                    boost::geometry::transform(other.get_point(), tempPoint2);

                    return boost::geometry::dot_product(tempPoint1, tempPoint2);

                }

                // returns the unit vector of current vector
                template <typename ReturnType>
                ReturnType unit_vector() const
                {
                    /*given coordinates/vectors are converted into cartesian and 
                    unit vector of it is returned by converting it into requested type*/

                    /*checking return type if they both are not subclass of
                    base_representaion then compile time erorr is generated*/
                    BOOST_STATIC_ASSERT_MSG((boost::is_base_template_of
                        <boost::astronomy::coordinate::base_representation, ReturnType>::value),
                        "return type is expected to be a representation class");
                    
                    boost::geometry::model::point<double, 3, boost::geometry::cs::cartesian> tempPoint;
                    double mag = this->magnitude();     //magnitude of vector stored in current object
                    boost::geometry::transform(this->point, tempPoint); //converting coordinate/vector into cartesian

                    //performing calculations to find unit vector
                    boost::geometry::set<0>(tempPoint, (boost::geometry::get<0>(tempPoint) / mag));
                    boost::geometry::set<1>(tempPoint, (boost::geometry::get<1>(tempPoint) / mag));
                    boost::geometry::set<2>(tempPoint, (boost::geometry::get<2>(tempPoint) / mag));

                    return ReturnType(tempPoint);
                }

                // converts current representation into specified representation
                template <typename ReturnType>
                ReturnType to_representation() const
                {
                    /*checking return type if they both are not subclass of
                    base_representaion then compile time erorr is generated*/
                    BOOST_STATIC_ASSERT_MSG((boost::is_base_template_of<boost::astronomy::coordinate::base_representation, ReturnType>),
                        "return type is expected to be a representation class");

                    return ReturnType(this->point);
                }

                // sum of current vector and specified vector
                template <typename ReturnType, typename Representation>
                ReturnType sum(Representation const& other) const
                { 
                    /*checking types of argument and return type if they both are not
                    subclass of base_representaion then compile time erorr is generated*/
                    BOOST_STATIC_ASSERT_MSG((boost::is_base_template_of
                        <boost::astronomy::coordinate::base_representation, Representation>::value),
                        "function argument type is expected to be a representation type");

                    BOOST_STATIC_ASSERT_MSG((boost::is_base_template_of
                        <boost::astronomy::coordinate::base_representation, ReturnType>::value),
                        "return type is expected to be a representation class");

                    /*converting both coordinates/vector into cartesian system*/
                    boost::geometry::model::point<double, 3, boost::geometry::cs::cartesian> tempPoint1, tempPoint2, result;
                    boost::geometry::transform(this->point, tempPoint1);
                    boost::geometry::transform(other.get_point(), tempPoint2);

                    //performing calculation to find the sum
                    boost::geometry::set<0>(result, (boost::geometry::get<0>(tempPoint1) + boost::geometry::get<0>(tempPoint2)));
                    boost::geometry::set<1>(result, (boost::geometry::get<1>(tempPoint1) + boost::geometry::get<1>(tempPoint2)));
                    boost::geometry::set<2>(result, (boost::geometry::get<2>(tempPoint1) + boost::geometry::get<2>(tempPoint2)));

                    return ReturnType(result);
                }
                
                // mean of current vector with specified vector
                template <typename ReturnType, typename Representation>
                ReturnType mean(Representation const& other) const
                {
                    /*checking types of argument and return type if they both are not
                    subclass of base_representaion then compile time erorr is generated*/
                    BOOST_STATIC_ASSERT_MSG((boost::is_base_template_of
                        <boost::astronomy::coordinate::base_representation, Representation>::value),
                        "function argument type is expected to be a representation type");

                    BOOST_STATIC_ASSERT_MSG((boost::is_base_template_of
                        <boost::astronomy::coordinate::base_representation, ReturnType>::value),
                        "return type is expected to be a representation class");

                    /*converting both coordinates/vector into cartesian system*/
                    boost::geometry::model::point<double, 3, boost::geometry::cs::cartesian> tempPoint1, tempPoint2, result;
                    boost::geometry::transform(this->point, tempPoint1);
                    boost::geometry::transform(other.get_point(), tempPoint2);

                    //performing calculation to find the mean
                    boost::geometry::set<0>(result, ((boost::geometry::get<0>(tempPoint1) + boost::geometry::get<0>(tempPoint2))/2));
                    boost::geometry::set<1>(result, ((boost::geometry::get<1>(tempPoint1) + boost::geometry::get<1>(tempPoint2))/2));
                    boost::geometry::set<2>(result, ((boost::geometry::get<2>(tempPoint1) + boost::geometry::get<2>(tempPoint2))/2));

                    return ReturnType(result);
                }

                // norm of the current class is returned
                double magnitude() const
                {
                    double result = 0.0;
                    boost::geometry::model::point<double, 3, boost::geometry::cs::cartesian> tempPoint;
                    boost::geometry::transform(this->point, tempPoint);
                    
                    /*for (int i = 0; i < DimensionCount; i++)
                    {
                        result += std::pow(boost::geometry::get<staticReturn(i)>(this->point), 2);
                    }*/

                    /*struct squaredsum
                    {
                        template <typename T>
                        int operator()(T)
                        {
                            return std::pow(boost::geometry::get<T::value>(point), 2);
                        }
                    };
                    
                    boost::mpl::for_each< boost::mpl::range_c<int, 0, dimensioncount>>(squaredsum());*/

                    return std::sqrt(result);
                }

                boost::geometry::model::point<double, DimensionCount, Type> get_point() const
                {
                    return this->point;
                }

            }; //base_representation
        } //namespace coordinate
    } //namespace astronomy
} //namespace boost
#endif // !BOOST_ASTRONOMY_COORDINATE_BASE_REPRESENTATION_HPP

