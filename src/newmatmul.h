#ifndef _newmatmul
#define _newmatmul


#include "EncryptedArray.h"



class MatMul1D {
public:
  virtual ~MatMul1D() {}
  virtual const EncryptedArray& getEA() const = 0;
  virtual long getDim() const = 0;
  virtual bool multipleTransforms() const = 0;
};

template<class type>
class MatMul1D_derived : public MatMul1D { 
public:
  PA_INJECT(type)

  // Should return true when the entry is a zero. 
  virtual bool get(RX& out, long i, long j, long k) const = 0;
};


class BlockMatMul1D {
public:
  virtual ~BlockMatMul1D() {}
  virtual const EncryptedArray& getEA() const = 0;
  virtual long getDim() const = 0;
  virtual bool multipleTransforms() const = 0;
};

template<class type>
class BlockMatMul1D_derived : public BlockMatMul1D { 
public:
  PA_INJECT(type)

  // Should return true when the entry is a zero. 
  virtual bool get(mat_R& out, long i, long j, long k) const = 0;
};



struct ConstMultiplier;

struct ConstMultiplierCache {
  std::vector<std::shared_ptr<ConstMultiplier>> multiplier;
  void upgrade(const FHEcontext& context);
};


class MatMulExecBase {
public:
  virtual ~MatMulExecBase() { }

  virtual const EncryptedArray& getEA() const = 0;
  virtual void upgrade() = 0;
  virtual void mul(Ctxt& ctxt) const = 0;
};


class MatMul1DExec : public MatMulExecBase {
public:

  const EncryptedArray& ea;
  bool minimal;

  long dim;
  long D;
  bool native;
  long g;

  ConstMultiplierCache cache;
  ConstMultiplierCache cache1; // only for non-native dimension


  // If minimal, then it is assumed minimal KS matrices will
  // be present (one for the generator g, and one for g^{-D} 
  // for non-native dimensions).  With this flag set, all BS/GS
  // and parallel strategies are avoided.
  explicit
  MatMul1DExec(const MatMul1D& mat, bool minimal=false);

  void mul(Ctxt& ctxt) const override;

  void upgrade() override { 
    cache.upgrade(ea.getContext()); 
    cache1.upgrade(ea.getContext()); 
  }

  const EncryptedArray& getEA() const override { return ea; }
};

class BlockMatMul1DExec : public MatMulExecBase {
public:

  const EncryptedArray& ea;

  long dim;
  long D;
  long d;
  bool native;
  long strategy;

  ConstMultiplierCache cache;
  ConstMultiplierCache cache1; // only for non-native dimension


  // If minimal, then it is assumed minimal KS matrices will be present (one
  // for Frobenius, one for the generator g, and one for g^{-D} for non-native
  // dimensions).  With this flag set, all BS/GS and parallel strategies are
  // avoided.
  explicit
  BlockMatMul1DExec(const BlockMatMul1D& mat, bool minimal=false);

  void mul(Ctxt& ctxt) const override;

  void upgrade() override { 
    cache.upgrade(ea.getContext()); 
    cache1.upgrade(ea.getContext()); 
  }

  const EncryptedArray& getEA() const override { return ea; }
};


void mul(NewPlaintextArray& pa, const MatMul1D& mat);
void mul(NewPlaintextArray& pa, const BlockMatMul1D& mat);


#endif
