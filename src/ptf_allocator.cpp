//
// Created by stone on 11/20/19.
//
#include "ptf_allocator.h"
#include <cstdio>

namespace portfolio {

void test_vector_ctpmd() {
  SharedCtpmdData cd;
  SharedCtpmdData cd2(100);
  CTPMD ctpmd;
  ctpmd.lp = 1;
  ctpmd.ap = 2;
  ctpmd.bp = 3;
  ctpmd.lv = 1;
  ctpmd.av = 2;
  ctpmd.bv = 3;
  ctpmd.ft = 54321;
  CTPMD& p = ctpmd;
  printf("1. %lf, %lf, %lf, %d, %d, %d, %lf\n", p.lp, p.ap, p.bp, p.lv, p.av, p.bv, p.ft);

  cd2.push_back(ctpmd);
  p = ctpmd;
  printf("2. %lf, %lf, %lf, %d, %d, %d, %lf\n", p.lp, p.ap, p.bp, p.lv, p.av, p.bv, p.ft);

  p = cd2[0];
  printf("3. %lf, %lf, %lf, %d, %d, %d, %lf\n", p.lp, p.ap, p.bp, p.lv, p.av, p.bv, p.ft);

  SharedCtpmdData cd3(cd2.get_data_ptr(), 100);
  printf("4. %lu, %lu\n", cd3.get_data_ptr(), cd2.get_data_ptr());
  p = *(cd3.get_data_ptr());
  printf("5. %lf, %lf, %lf, %d, %d, %d, %lf\n", p.lp, p.ap, p.bp, p.lv, p.av, p.bv, p.ft);
  cd3.resize(1);
  p = cd3[0];
  printf("6. %lf, %lf, %lf, %d, %d, %d, %lf\n", p.lp, p.ap, p.bp, p.lv, p.av, p.bv, p.ft);
  p = cd2[0];
  printf("7. %lf, %lf, %lf, %d, %d, %d, %lf\n", p.lp, p.ap, p.bp, p.lv, p.av, p.bv, p.ft);

};

} //namespace protfolio {
