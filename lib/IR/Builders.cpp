//===- Builders.cpp - Helpers for constructing MLIR Classes ---------------===//
//
// Copyright 2019 The MLIR Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// =============================================================================

#include "mlir/IR/Builders.h"
#include "mlir/IR/AffineExpr.h"
#include "mlir/IR/AffineMap.h"
#include "mlir/IR/Attributes.h"
#include "mlir/IR/IntegerSet.h"
#include "mlir/IR/Module.h"
#include "mlir/IR/Types.h"
using namespace mlir;

Builder::Builder(Module *module) : context(module->getContext()) {}

Identifier Builder::getIdentifier(StringRef str) {
  return Identifier::get(str, context);
}

Module *Builder::createModule() { return new Module(context); }

//===----------------------------------------------------------------------===//
// Types.
//===----------------------------------------------------------------------===//

FloatType *Builder::getBF16Type() { return Type::getBF16(context); }

FloatType *Builder::getF16Type() { return Type::getF16(context); }

FloatType *Builder::getF32Type() { return Type::getF32(context); }

FloatType *Builder::getF64Type() { return Type::getF64(context); }

OtherType *Builder::getAffineIntType() { return Type::getAffineInt(context); }

OtherType *Builder::getTFControlType() { return Type::getTFControl(context); }

OtherType *Builder::getTFStringType() { return Type::getTFString(context); }

IntegerType *Builder::getIntegerType(unsigned width) {
  return Type::getInteger(width, context);
}

FunctionType *Builder::getFunctionType(ArrayRef<Type *> inputs,
                                       ArrayRef<Type *> results) {
  return FunctionType::get(inputs, results, context);
}

MemRefType *Builder::getMemRefType(ArrayRef<int> shape, Type *elementType,
                                   ArrayRef<AffineMap *> affineMapComposition,
                                   unsigned memorySpace) {
  return MemRefType::get(shape, elementType, affineMapComposition, memorySpace);
}

VectorType *Builder::getVectorType(ArrayRef<unsigned> shape,
                                   Type *elementType) {
  return VectorType::get(shape, elementType);
}

RankedTensorType *Builder::getTensorType(ArrayRef<int> shape,
                                         Type *elementType) {
  return RankedTensorType::get(shape, elementType);
}

UnrankedTensorType *Builder::getTensorType(Type *elementType) {
  return UnrankedTensorType::get(elementType);
}

//===----------------------------------------------------------------------===//
// Attributes.
//===----------------------------------------------------------------------===//

BoolAttr *Builder::getBoolAttr(bool value) {
  return BoolAttr::get(value, context);
}

IntegerAttr *Builder::getIntegerAttr(int64_t value) {
  return IntegerAttr::get(value, context);
}

FloatAttr *Builder::getFloatAttr(double value) {
  return FloatAttr::get(value, context);
}

StringAttr *Builder::getStringAttr(StringRef bytes) {
  return StringAttr::get(bytes, context);
}

ArrayAttr *Builder::getArrayAttr(ArrayRef<Attribute *> value) {
  return ArrayAttr::get(value, context);
}

AffineMapAttr *Builder::getAffineMapAttr(AffineMap *value) {
  return AffineMapAttr::get(value, context);
}

TypeAttr *Builder::getTypeAttr(Type *type) {
  return TypeAttr::get(type, context);
}

FunctionAttr *Builder::getFunctionAttr(Function *value) {
  return FunctionAttr::get(value, context);
}

//===----------------------------------------------------------------------===//
// Affine Expressions, Affine Maps, and Integet Sets.
//===----------------------------------------------------------------------===//

AffineMap *Builder::getAffineMap(unsigned dimCount, unsigned symbolCount,
                                 ArrayRef<AffineExpr *> results,
                                 ArrayRef<AffineExpr *> rangeSizes) {
  return AffineMap::get(dimCount, symbolCount, results, rangeSizes, context);
}

AffineDimExpr *Builder::getDimExpr(unsigned position) {
  return AffineDimExpr::get(position, context);
}

AffineSymbolExpr *Builder::getSymbolExpr(unsigned position) {
  return AffineSymbolExpr::get(position, context);
}

AffineConstantExpr *Builder::getConstantExpr(int64_t constant) {
  return AffineConstantExpr::get(constant, context);
}

AffineExpr *Builder::getAddExpr(AffineExpr *lhs, AffineExpr *rhs) {
  return AffineBinaryOpExpr::get(AffineExpr::Kind::Add, lhs, rhs, context);
}

AffineExpr *Builder::getMulExpr(AffineExpr *lhs, AffineExpr *rhs) {
  return AffineBinaryOpExpr::get(AffineExpr::Kind::Mul, lhs, rhs, context);
}

AffineExpr *Builder::getModExpr(AffineExpr *lhs, AffineExpr *rhs) {
  return AffineBinaryOpExpr::get(AffineExpr::Kind::Mod, lhs, rhs, context);
}

AffineExpr *Builder::getFloorDivExpr(AffineExpr *lhs, AffineExpr *rhs) {
  return AffineBinaryOpExpr::get(AffineExpr::Kind::FloorDiv, lhs, rhs, context);
}

AffineExpr *Builder::getCeilDivExpr(AffineExpr *lhs, AffineExpr *rhs) {
  return AffineBinaryOpExpr::get(AffineExpr::Kind::CeilDiv, lhs, rhs, context);
}

IntegerSet *Builder::getIntegerSet(unsigned dimCount, unsigned symbolCount,
                                   ArrayRef<AffineExpr *> constraints,
                                   ArrayRef<bool> isEq) {
  return IntegerSet::get(dimCount, symbolCount, constraints, isEq, context);
}

//===----------------------------------------------------------------------===//
// CFG function elements.
//===----------------------------------------------------------------------===//

// Basic block.
BasicBlock *CFGFuncBuilder::createBlock() {
  BasicBlock *b = new BasicBlock();
  function->push_back(b);
  setInsertionPoint(b);
  return b;
}

/// Create an operation given the fields represented as an OperationState.
OperationInst *CFGFuncBuilder::createOperation(const OperationState &state) {
  SmallVector<CFGValue *, 8> operands;
  operands.reserve(state.operands.size());
  for (auto elt : state.operands)
    operands.push_back(cast<CFGValue>(elt));

  auto *op = OperationInst::create(state.name, operands, state.types,
                                   state.attributes, context);
  block->getOperations().insert(insertPoint, op);
  return op;
}

//===----------------------------------------------------------------------===//
// Statements.
//===----------------------------------------------------------------------===//

/// Create an operation given the fields represented as an OperationState.
OperationStmt *MLFuncBuilder::createOperation(const OperationState &state) {
  SmallVector<MLValue *, 8> operands;
  operands.reserve(state.operands.size());
  for (auto elt : state.operands)
    operands.push_back(cast<MLValue>(elt));

  auto *op = OperationStmt::create(state.name, operands, state.types,
                                   state.attributes, context);
  block->getStatements().insert(insertPoint, op);
  return op;
}

ForStmt *MLFuncBuilder::createFor(AffineConstantExpr *lowerBound,
                                  AffineConstantExpr *upperBound,
                                  AffineConstantExpr *step) {
  if (!step)
    step = getConstantExpr(1);
  auto *stmt = new ForStmt(lowerBound, upperBound, step, context);
  block->getStatements().insert(insertPoint, stmt);
  return stmt;
}
