/*
 * QIC_lib (Quantum information and computation library)
 *
 * Copyright (c) 2015 - 2016  Titas Chanda (titas.chanda@gmail.com)
 *
 * This file is part of QIC_lib.
 *
 * QIC_lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * QIC_lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with QIC_lib.  If not, see <http://www.gnu.org/licenses/>.
 */



namespace qic 
{

  template<typename T1, typename T2>
  inline 
  typename std::enable_if< 
    std::is_arithmetic< pT<T1> >::value
  && std::is_arithmetic< pT<T2> >::value
  && is_comparable_pT<T1,T2>::value,
    arma::Mat< typename eT_promoter_var<T1,T2>::type >  
    >::type apply(const T1& rho1, 
		       const T2& A, 
		       arma::uvec sys, 
		       arma::uvec dim)
  {

    const auto& p = as_Mat(rho1); 
    const auto& A1 = as_Mat(A);
    
    bool checkV = true;
    if(p.n_cols == 1)
      checkV = false;


#ifndef QIC_LIB_NO_DEBUG
    if(p.n_elem == 0)
      throw Exception("qic::apply",Exception::type::ZERO_SIZE);

    if(A1.n_elem == 0)
      throw Exception("qic::apply",Exception::type::ZERO_SIZE);

    if(checkV)
      if(p.n_rows!=p.n_cols)
	throw Exception("qic::apply",
			Exception::type::MATRIX_NOT_SQUARE_OR_CVECTOR);
    
    if(A1.n_rows!=A1.n_cols)
      throw Exception("qic::apply",Exception::type::MATRIX_NOT_SQUARE);
    
    if( dim.n_elem == 0 || arma::any(dim == 0))
      throw Exception("qic::apply",Exception::type::INVALID_DIMS);

    if( arma::prod(dim)!= p.n_rows)
      throw Exception("qic::apply",Exception::type::DIMS_MISMATCH_MATRIX);

    if( arma::prod(dim(sys-1)) != A.n_rows )
      throw Exception("qic::apply",
		      Exception::type::MATRIX_MISMATCH_SUBSYS);

    if(sys.n_elem > dim.n_elem 
       || arma::find_unique(sys).eval().n_elem != sys.n_elem 
       || arma::any(sys > dim.n_elem) || arma::any(sys == 0) )
      throw Exception("qic::apply",Exception::type::INVALID_SUBSYS);      
#endif
   
    return apply_ctrl(p,A1,{},std::move(sys),std::move(dim));
    

  }


  template<typename T1, typename T2>
  inline  
  typename std::enable_if< 
    std::is_arithmetic< pT<T1> >::value
  && std::is_arithmetic< pT<T2> >::value
  && is_comparable_pT<T1,T2>::value,
    arma::Mat< typename eT_promoter_var<T1,T2>::type >   
    >::type apply(const T1& rho1, 
		  const T2& A, 
		  arma::uvec sys, 
		  arma::uword dim = 2)
  {
    const auto& rho = as_Mat(rho1); 
    
    bool checkV = true;
    if(rho.n_cols == 1)
      checkV = false;

#ifndef QIC_LIB_NO_DEBUG
    if(rho.n_elem == 0)
      throw Exception("qic::apply",Exception::type::ZERO_SIZE);

    if(checkV)
      if(rho.n_rows != rho.n_cols)
	throw Exception("qic::apply",
			Exception::type::MATRIX_NOT_SQUARE_OR_CVECTOR);
    
    if(dim == 0)
      throw Exception("qic::apply",Exception::type::INVALID_DIMS);

    if( arma::prod(dim)!= rho.n_rows)
      throw Exception("qic::apply",Exception::type::DIMS_MISMATCH_MATRIX);
#endif
  
    arma::uword n = static_cast<arma::uword>
      (std::llround(std::log(rho.n_rows) / std::log(dim)));

    arma::uvec dim2 = dim * arma::ones<arma::uvec>(n);
    return apply(rho,A,std::move(sys),std::move(dim2));

  }


}