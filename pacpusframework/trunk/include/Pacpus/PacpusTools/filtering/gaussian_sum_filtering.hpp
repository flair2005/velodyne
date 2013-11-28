// %pacpus:license{
// This file is part of the PACPUS framework distributed under the
// CECILL-C License, Version 1.0.
// %pacpus:license}
/// @file
/// @author  Firstname Surname <firstname.surname@utc.fr>
/// @date    Month, Year
/// @version $Id: gaussian_sum_filtering.hpp 76 2013-01-10 17:05:10Z kurdejma $
/// @copyright Copyright (c) UTC/CNRS Heudiasyc 2006 - 2013. All rights reserved.
/// @brief Brief description.
///
/// Detailed description.

#ifndef __GAUSSIAN_FILTERING_BASE__
#define __GAUSSIAN_FILTERING_BASE__

#include "kalman_filtering.hpp"
#include "../math/pdf.hpp"

#include <vector>

namespace filter{

namespace gaussiansum{

   using namespace math;
//   using namespace rng;
   using namespace ublas;

  /*!
   * \class Gaussian
   * \brief This class describes a basic weighted kalman state
   */
  class Gaussian : public filter::kalman::State{
     public :

     /*!
      * \brief Constructor
      * \param state_size : Size of the state vector 
      * \param weight_ : Initial weight
      */
    Gaussian(const size_t &state_size, const double &weight_){
       filter::kalman::State::Allocator(state_size);
       weight=weight_;
     }

     /**
      * \brief Constructor
      * \param state_size : Size of the state vector 
      */

     Gaussian(const size_t &state_size){
       filter::kalman::State::Allocator(state_size);
       weight=0;
     }

     double weight; /*!< Weight of the Kalman state */

/*
    protected :*/

//      Gaussian:weight(1){};
  };


  /*!
   * \class GaussianSet
   * \brief  This method describes a set of gaussians \n
   *  A set of gaussains is reprented by a vector of weighted kalman state \n 
   *  somes methods can be applied to the set of particles like :\n 
   *  estimate computation, resampling scheme or normalization method \n
   */
  template<class G=Gaussian> class GaussianSet {
    public :

      /** Normalize the weights of gaussian states*/
      void NormalizeWeights();

      /*!
      * \brief Allocate the set of gaussians  
      * \param ngaussian : number of gaussians in the set
      */  
      void Allocator(const size_t &ngaussian);

      /*!
       * \brief Compute the estimate
       *  \return Vector
       */  
      Vector Estimate();

      /*!
       * \brief Destructor 
       */
      ~GaussianSet(){}


      std::vector<G>  gaussians; /*!< gaussian states */


    };


     template<class G> void GaussianSet<G>::NormalizeWeights(){
        double sum=0;
        for(typename std::vector<G>::iterator I=gaussians.begin();I!=gaussians.end();I++)
          sum+=(*I).weight;

        if(sum==0){throw filter_error("Gaussian set normalization weight sum =0");}
        for(typename std::vector<G>::iterator I=gaussians.begin();I!=gaussians.end();I++)
           (*I).weight/=sum;
      }

      
      template<class G> void GaussianSet<G>::Allocator(const size_t &ngaussian){
        gaussians.reserve(ngaussian);
      } 

     template<class G> Vector GaussianSet<G>::Estimate(){
       Vector estimate = ZeroVector(gaussians[0].X.size());
       for(typename std::vector<G>::iterator I=gaussians.begin();I!=gaussians.end();I++)
         estimate+=(*I).X*(*I).weight;
       return estimate;
     }


  /*!
   * \class LinearDynamicEquation
   * \brief This class describes a linear dynamic equation 
   */
  template <template <class> class S=GaussianSet, class G=Gaussian > class LinearDynamicEquation:public filter::kalman::LinearDynamicEquation<G>{
   public :
       /*!
        * \brief virtual method where parameters of the dynamic equation must be  evaluated 
        * \param s : weighted kalman state at time k-1
        */
        virtual void EvaluateParameters(G *s)=0;

        /*!
        * \brief virtual method where the a posteriori state vector must be computed
        * \param in : the a posteriori set of gaussian at time k-1 
        * \param out : the a priori set of gaussian at time k 
        */  
        virtual void Predict(S<G> *in,S<G> *out);

       /*!
        * \brief Destructor 
        */
        virtual ~LinearDynamicEquation(){}
   };


       template <template <class> class S, class G> void LinearDynamicEquation<S,G>::Predict(S<G> *in,S<G> *out){
          for(size_t i=0;i<in->gaussians.size();i++){
            EvaluateParameters(&in->gaussians[i]); 

            out->gaussians[i].P= filter::kalman::LinearDynamicEquation<G>::_A*in->gaussians[i].P*Trans( filter::kalman::LinearDynamicEquation<G>::_A)
                                 +filter::kalman::LinearDynamicEquation<G>::_B*filter::kalman::LinearDynamicEquation<G>::_Q*Trans(filter::kalman::LinearDynamicEquation<G>::_B);
            out->gaussians[i].X= filter::kalman::LinearDynamicEquation<G>::_A*in->gaussians[i].X
                                 +filter::kalman::LinearDynamicEquation<G>::_B*filter::kalman::LinearDynamicEquation<G>::_U;
            out->gaussians[i].weight=in->gaussians[i].weight;
          }
      }


  /*!
   * \class NonLinearDynamicEquation
   * \brief This class describes a non linear dynamic equation 
   */
   template <template <class> class S=GaussianSet, class G=Gaussian > class NonLinearDynamicEquation:public filter::kalman::NonLinearDynamicEquation<G>{
   public :
       /*!
        * \brief virtual method where parameters of the dynamic equation must be  evaluated 
        * \param s : weighted kalman state at time k-1
        * f= \n
        * F= \n
        * G= \n
        */
        virtual void EvaluateParameters(G *s)=0;

       /*!
        * \brief virtual method where the a posteriori state vector must be computed
        * \param in : the a posteriori set of gaussian at time k-1 
        * \param out : the a priori set of gaussian at time k 
        */  
      virtual void Predict(S<G> *in,S<G> *out);

      /*!
       * \brief Destructor 
       */
      virtual ~NonLinearDynamicEquation(){}
  };


       template <template <class> class S, class G>  void NonLinearDynamicEquation<S,G>::Predict(S<G> *in,S<G> *out){
        for(size_t i=0;i<in->gaussians.size();i++){
            EvaluateParameters(&in->gaussians[i]); 

            out->gaussians[i].P=filter::kalman::NonLinearDynamicEquation<G>::_F*in->gaussians[i].P*Trans(filter::kalman::NonLinearDynamicEquation<G>::_F)
                                +filter::kalman::NonLinearDynamicEquation<G>::_G*filter::kalman::NonLinearDynamicEquation<G>::_Q*Trans(filter::kalman::NonLinearDynamicEquation<G>::_G);
            out->gaussians[i].X=filter::kalman::NonLinearDynamicEquation<G>::_f;
            out->gaussians[i].weight=in->gaussians[i].weight;
         }
      }

  /*!
   * \class LinearMeasureEquation
   * \brief This class describes a linear measure equation 
   */  
   template <template <class> class S=GaussianSet, class G=Gau
ssian >class LinearMeasureEquation:public filter::kalman::LinearMeasureEquation<G>{
   public :

        /*!
        * \brief virtual method where parameters of the measure equation must be  evaluated 
        * \param s : weighted kalman state at time k-1
        */
      virtual void EvaluateParameters(G *s)=0;
     

       /*!
        * \brief virtual method where the a posteriori state vector must be computed
        * \param in : the a priori set of gaussian at time k 
        * \param out : the a posteriori set of gaussian at time k 
        */  
        virtual void Update(S<G> *in,S<G> *out);


 

       /*! 
        * \brief Destructor 
        */ 
        virtual ~LinearMeasureEquation(){};

      protected :

       /*!
        * \brief virtual method where likelihood value for each particle is computed
        * \param s :  a priori weighted kalman state at time k
        * \return likelihood value
        */
        virtual double ZPDF(G *s);

 };


      template <template <class> class S, class G>  void LinearMeasureEquation<S,G>::Update(S<G> *in,S<G> *out){ 
  
         for(size_t i=0;i<in->gaussians.size();i++){
            filter::kalman::LinearMeasureEquation<G>::_coherency=true;
            EvaluateParameters(&in->gaussians[i]); 
   
           if(filter::kalman::LinearMeasureEquation<G>::_coherency){
              filter::kalman::LinearMeasureEquation<G>::_K=in->gaussians[i].P*Trans(filter::kalman::LinearMeasureEquation<G>::_H) *
                ( InvQR(filter::kalman::LinearMeasureEquation<G>::_H*in->gaussians[i].P*Trans(filter::kalman::LinearMeasureEquation<G>::_H)+filter::kalman::LinearMeasureEquation<G>::_R) );
               out->gaussians[i].X=in->gaussians[i].X+filter::kalman::LinearMeasureEquation<G>::_K*(filter::kalman::LinearMeasureEquation<G>::_Z-filter::kalman::LinearMeasureEquation<G>::_H*in->gaussians[i].X);
               out->gaussians[i].P=in->gaussians[i].P-filter::kalman::LinearMeasureEquation<G>::_K*filter::kalman::LinearMeasureEquation<G>::_H*in->gaussians[i].P;
           }   
        
           out->gaussians[i].weigh=ZPDF(&in->gaussian[i])*in->gaussians[i].weight;

         }

         //out->NormalizeWeights();
      }

     template <template <class> class S, class G> double LinearMeasureEquation<S,G>::ZPDF(G *s){
        Vector apvec=filter::kalman::LinearMeasureEquation<G>::_H*s->X;
        Matrix apcov=filter::kalman::LinearMeasureEquation<G>::_H*s->P*Trans(filter::kalman::LinearMeasureEquation<G>::_H)+filter::kalman::LinearMeasureEquation<G>::_R;
        Matrix apinv=InvQR(apcov);
        double apdet=Det(apcov);
        return (std::exp(-0.5*Dot(filter::kalman::LinearMeasureEquation<G>::_Z-apvec,apinv*(filter::kalman::LinearMeasureEquation<G>::_Z-apvec)))/
               (std::sqrt(std::pow(2*M_PI,static_cast<int>(apvec.size()))*std::abs(apdet))));
     }


  /*!
   * \class NonLinearMeasureEquation
   * \brief This class describes a non linear measure equation 
   */  
 template <template <class> class S=GaussianSet, class G=Gaussian > class NonLinearMeasureEquation:public filter::kalman::NonLinearMeasureEquation<G>{
   public :
        /*!
        * \brief virtual method where parameters of the measure equation must be  evaluated 
        * \param s : weighted kalman state at time k-1
        * h= \n
        * H= \n
        */
        virtual void EvaluateParameters(G *s )=0;

       /*!
        * \brief virtual method where the a posteriori state vector must be computed
        * \param in : the a priori set of gaussian at time k 
        * \param out : the a posteriori set of gaussian at time k 
        */  
        virtual void Update(S<G> *in,S<G> *out);
      
       /*!
        * \brief  destructor 
        */
        virtual ~NonLinearMeasureEquation(){}

     protected :

       /*!
        * \brief virtual method where likelihood value for each particle is computed
        * \param s :  a priori weighted kalman state at time k
        * \return likelihood value
        */
        virtual  double ZPDF(G *s);

  };

     template <template <class> class S, class G> void NonLinearMeasureEquation<S,G>::Update(S<G> *in,S<G> *out){

        for(size_t i=0;i<in->gaussians.size();i++){
            filter::kalman::NonLinearMeasureEquation<G>::_coherency=true;
            EvaluateParameters(&in->gaussians[i]); 

            if(filter::kalman::NonLinearMeasureEquation<G>::_coherency){
               filter::kalman::NonLinearMeasureEquation<G>::_K=in->gaussians[i].P*Trans(filter::kalman::NonLinearMeasureEquation<G>::_H)*(InvQR(filter::kalman::NonLinearMeasureEquation<G>::_H*in->gaussians[i].P*Trans(filter::kalman::NonLinearMeasureEquation<G>::_H) + filter::kalman::NonLinearMeasureEquation<G>::_R));
                out->gaussians[i].X=in->gaussians[i].X+filter::kalman::NonLinearMeasureEquation<G>::_K*(filter::kalman::NonLinearMeasureEquation<G>::_Z-filter::kalman::NonLinearMeasureEquation<G>::_h);
                out->gaussians[i].P=in->gaussians[i].P-filter::kalman::NonLinearMeasureEquation<G>::_K*filter::kalman::NonLinearMeasureEquation<G>::_H*in->gaussians[i].P;
            }
            out->gaussians[i].weight=ZPDF(&in->gaussians[i])*in->gaussians[i].weight;
         }
 
         //out->NormalizeWeights();
     }

     template <template <class> class S, class G> double NonLinearMeasureEquation<S,G>::ZPDF(G *s){
        Vector apvec=filter::kalman::NonLinearMeasureEquation<G>::_H*s->X;
        Matrix apcov=filter::kalman::NonLinearMeasureEquation<G>::_H*s->P*Trans(filter::kalman::NonLinearMeasureEquation<G>::_H)+filter::kalman::NonLinearMeasureEquation<G>::_R;
        Matrix apinv=InvQR(apcov);
        double apdet=Det(apcov);
        return (std::exp(-0.5*Dot(filter::kalman::NonLinearMeasureEquation<G>::_Z-apvec,apinv*(filter::kalman::NonLinearMeasureEquation<G>::_Z-apvec)))/
               (std::sqrt(std::pow(2*M_PI,static_cast<int>(apvec.size()))*std::abs(apdet))));
     }

} // namespace gaussiansum
} // namespace filter

#endif // __GAUSSIAN_FILTERING_BASE__