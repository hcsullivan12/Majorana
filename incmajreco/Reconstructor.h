/**
 * @file Reconstructor.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief The SiPM wheel reconstruction algorithm.
 * @date 07-04-2019
 * 
 */

#ifndef MAJRECO_RECONSTRUCTOR_H
#define MAJRECO_RECONSTRUCTOR_H

#include "majutil/Pixel.h"

#include <map>
#include <list>
#include <memory>

#include "TH2.h"
#include "TF2.h"

namespace majreco
{

class Reconstructor 
{

public:

  /**
   * @brief Contructor, intialized or pixel containers.
   * 
   */
  Reconstructor();
  ~Reconstructor();

  /**
   * @brief Initialize our container and parameters for reconstruction.
   * 
   * @param data Map from SiPM ID to measured counts.
   * @param pixelVec Pointer to the pixels.
   * @param diskRadius Used for plotting.
   * @param gamma Strength parameter for penalty function (default 0).
   * @param pStop Iteration number to stop penalized reconstruction.
   * @param upStop Iteration number to stop unpenalized reconstruction.
   */
  void Initialize(const std::map<size_t, size_t>&     data,
                  std::shared_ptr<std::vector<Pixel>> pixelList,
                  const float&                        diskRadius,
                  const float&                        gamma, 
                  const size_t&                       pStop,
                  const size_t&                       upStop);

  /**
   * @brief Entry point to reconstruction algorithm.
   * 
   * There are two reconstruction stages: unpenalized and penalized.
   * The current method is to always run the unpenalized reconstruction.
   * The user has the option to subsequently run the penalized reconstruction
   * using information from the unpenalized reconstruction.
   * 
   * @param doPenalized Option to run penalized reconstruction (default true).
   */  
  void Reconstruct(const bool& doPenalized = true);                  

  /**
   * @brief Dump configuration and reconstruction results.
   * 
   */
  void Dump();

  const double   ML()    const { return fMLLogLikelihood; }
  const float    X()     const { return fMLX; }
  const float    Y()     const { return fMLY; }
  TH2F*          MLImage() { return fMLHistogram; }
    
private:
  
  /**
   * @brief Simple factorial function.
   *
   */
  inline int doFactorial(const size_t& n) 
  { 
    if (n <= 1) return 1;
    return n * doFactorial(n-1);
  }

  /**
   * @brief Do unpenalized version of reconstruction.
   * 
   * The procedure is as follows:
   * 1) Make initial estimates for pixel intensities
   * 2) Make next prediction
   * 3) Check for convergence
   *      *if yes, save, return
   *      *if not, old estimates = current estimates -> 2)
   * 
   */
  void DoUnpenalized();

  /**
   * @brief Do penalized version of reconstruction. Same as 
   *        unpenalized except uses different formula to update
   *        estimates.
   * 
   */
  void DoPenalized();

  /**
   * @brief Initializes the pixel intensities using a uniform distribution.
   * 
   */
  void InitPixelList();

  /**
   * @brief Calculates the unpenalized pixel values for the image.
   * 
   */
  void UnpenalizedEstimate();

  /**
   * @brief Calculates the penalized pixel values for the image.
   * 
   */
  void PenalizedEstimate();

  /**
   * @brief Calulate log likelihood using current estimates.
   * @todo Clean up this area.
   * 
   */
  float CalculateLL();
  float CalculateMean(const size_t& sipmID);

  /**
   * @brief Calculates the sum in the denominator of the Money Formula.
   * 
   */
  float DenominatorSum(const size_t& sipmID);

  /**
   * @brief Entry point for applying the Moneyu Formula.
   * 
   * @param pixelID The pixel ID to update.
   * @param theEst The current estimate.
   * @param referenceTable The lookup table for this pixel.
   * @return float The updated pixel intensity.
   */
  float MoneyFormula(const float&              theEst,
                     const std::vector<float>& referenceTable);

  /**
   * @brief Method to check for convergence. Currently returns true.
   * @todo Finish writing.
   * 
   * @return true If converged.
   * @return false If not.
   */
  bool CheckConvergence();

  /**
   * @brief Resets the pixel estimates with the updated estimates.
   * 
   */
  void Reset();

  /**
   * @brief Updates the ML histogram with current estimates. Fits 
   *        distribution using a 2D gaussian function to find the
   *        mean x and y coordinates.
   * 
   */
  void UpdateHistogram();

  /**
   * @brief Updates the priors used in the penalized reconstruction.
   * 
   */
  void InitializePriors();
   
  TH2F*                        fMLHistogram;           ///< The reconstructed image
  TF2*                         fMLGauss;               ///< Gaussian fit to point source
  double                       fMLLogLikelihood;       ///< Log likelihood for the MLE
  float                        fMLTotalLight;          ///< MLE for total light
  float                        fMLX;                   ///< MLE for x (cm)
  float                        fMLY;                   ///< MLE for y (cm)
  float                        fDiskRadius;            ///< Disk radius 
  std::shared_ptr<std::vector<Pixel>> fPixelVec;       ///< List of pixels
  std::vector<float>           fDenomSums;             ///< Map of sipm to denominator sum
  std::map<size_t, size_t>     fData;                      ///< Measured counts (sipm, np.e.)
  std::vector<float>           fLogLikehs;             ///< Container for logL at each iteration 
  std::vector<float>           fPriors;                ///< Container for priors used in penalized reconstruction
  float                        fGamma;                 ///< Strength parameter for penalty function
  size_t                       fUnpenalizedIterStop;   ///< Iteration number to hault unpenalized reconstruction
  size_t                       fPenalizedIterStop;     ///< Iteration number to hault penalized reconstruction
};
}

#endif
