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

  namespace
  {

    inline void set_discord3_reg_prob_tol(double a){protect::_discord3_reg_prob_tol = a;}

    template<typename T1>
    typename arma::Col<typename T1::pod_type>::template fixed<3> discord3_reg(const T1& rho1,arma::uword nodal, arma::uvec dim)
    {
      typedef typename T1::pod_type pT; 

      const auto& rho = as_Mat(rho1);
      arma::uword party_no = dim.n_elem;
      arma::uword dim1 = arma::prod(dim);

#ifndef QIC_LIB_NO_DEBUG   
      if(rho.n_elems == 0)
	throw Exception("qic::discord3_reg", Exception::type::ZERO_SIZE);

      if(rho.n_rows!=rho.n_cols)
	throw Exception("qic::discord3_reg",Exception::type::MATRIX_NOT_SQUARE);
     
      if(any(dim == 0))
	throw Exception("qic::discord3_reg",Exception::type::INVALID_DIMS);
      
      if( dim1 != rho.n_rows)
	throw Exception("qic::discord3_reg", Exception::type::DIMS_MISMATCH_MATRIX);

      if(nodal<=0 || nodal > party_no)
	throw Exception("qic::discord3_reg", "Invalid measured party index");

      if( dim(nodal-1) != 3 )
	throw Exception("qic::discord3_reg", "Measured party is not qutrit");
#endif

      
      arma::uvec party=arma::zeros<arma::uvec>(party_no); 
      for(arma::uword i=0; i<party_no ; ++i)
	party.at(i)=i+1;
  

      arma::uvec rest = party;
      rest.shed_row(nodal-1);



      auto rho_A = TrX(rho,rest,dim);    
      auto rho_B = TrX(rho,{nodal},dim); 


      auto S_A = entropy(rho_A);
      auto S_B = entropy(rho_B); 
      auto S_A_B = entropy(rho); 
      auto I1 = S_A + S_B - S_A_B; 

      dim1 /= 3;
      arma::uword dim2 (1);
      for(arma::uword i = 0 ;i < nodal-1 ; ++i)
	dim2 *= dim.at(i);
      arma::uword dim3 (1);
      for(arma::uword i = nodal ; i < party_no ; ++i)
	dim3 *= dim.at(i);
	  
      arma::Mat<pT> eye2 = arma::eye< arma::Mat<pT> >(dim1,dim1);
      arma::Mat<pT> eye3 = arma::eye< arma::Mat<pT> >(dim2,dim2);
      arma::Mat<pT> eye4 = arma::eye< arma::Mat<pT> >(dim3,dim3);

      typename arma::Col<pT>::template fixed<3> disc;

      for(arma::uword i=0;i<3;++i)
	{
      
	  arma::Mat< std::complex<pT> > proj1 = STATES<pT>::get_instance().proj3.at(0,i+1);
	  arma::Mat< std::complex<pT> > proj2 = STATES<pT>::get_instance().proj3.at(1,i+1);
	  arma::Mat< std::complex<pT> > proj3 = STATES<pT>::get_instance().proj3.at(2,i+1);
  
	  if(nodal==1)
	    {
	      proj1 = kron(proj1,eye2 );
	      proj2 = kron(proj2,eye2 );
	      proj3 = kron(proj3,eye2 );
	    }
	  else if (party_no==nodal)
	    {
	      proj1 = kron(eye2,proj1);
	      proj2 = kron(eye2,proj2);
	      proj3 = kron(eye2,proj3);
	    }
	  else
	    {
	      proj1 = kron(kron(eye3,proj1),eye4);
	      proj2 = kron(kron(eye3,proj2),eye4);
	      proj3 = kron(kron(eye3,proj3),eye4);
	    }
  
  
  

	  arma::Mat< std::complex<pT> > rho_1 = (proj1*rho*proj1);
	  arma::Mat< std::complex<pT> > rho_2 = (proj2*rho*proj2);
	  arma::Mat< std::complex<pT> > rho_3 = (proj3*rho*proj3);
 

	  pT p1 = std::real(arma::trace(rho_1));
	  pT p2 = std::real(arma::trace(rho_2));
	  pT p3 = std::real(arma::trace(rho_3));


	  pT S_max =0.0;
	  if( p1 > static_cast<pT>(protect::_discord3_reg_prob_tol) )
	    {
	      rho_1 /= p1;
	      S_max += p1 * entropy(rho_1);
	    }
	  if( p2 > static_cast<pT>(protect::_discord3_reg_prob_tol) )
	    {
	      rho_2 /= p2;
	      S_max += p2 * entropy(rho_2);
	    }
	  if( p3 > static_cast<pT>(protect::_discord3_reg_prob_tol) ) 
	    {
	      rho_3 /= p3;
	      S_max += p3 * entropy(rho_3);
	    }
	  disc.at(i) = I1 -(S_B-S_max);

	}

      return(disc);
    }
  }



}