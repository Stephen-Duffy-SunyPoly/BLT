#pragma once

#include <memory>
#include <utility>

enum DataTypeValues {
    VOID_TYPE_VALUE = 0,
    INT_TYPE_VALUE = 1,
    FIXED_TYPE_VALUE = 2,
    POINTER_TYPE_VALUE = 3
};

class DataType {
public:
    virtual ~DataType() = default;

    /**Get the unique id of this data type
     * @return The unique id of this data type
     */
    virtual int getTypeId() = 0;

    /**Check if this type is exactly equal to other type
     * @param other The other data type to check
     * @return true if the data types are exactly the same
     */
    virtual bool typeEqual(DataType* other) = 0;

    /**Check if this data type is a pointer type
     * @return true if this data type is a pointer
     */
    virtual bool isPointer() {
        return false;
    }

    /**Get a pointer to the subtype of this data type
     * @return A pointer to the sub type of this data type or null if there is not sub type
     */
    virtual DataType* getSubType() {
        return nullptr;
    }
};

class VoidType : public DataType {
    const int TYPE_ID = VOID_TYPE_VALUE;
public:
    int getTypeId() override {
        return TYPE_ID;
    }
    bool typeEqual(DataType* other) override {
        return TYPE_ID == other->getTypeId();
    }
};

class IntType : public DataType {
    const int TYPE_ID = INT_TYPE_VALUE;
public:
    int getTypeId() override {
        return TYPE_ID;
    }
    bool typeEqual(DataType* other) override {
        return TYPE_ID == other->getTypeId();
    }
};

class FixedType : public DataType {
    const int TYPE_ID = FIXED_TYPE_VALUE;
public:
    int getTypeId() override {
        return TYPE_ID;
    }
    bool typeEqual(DataType* other) override {
        return TYPE_ID == other->getTypeId();
    }
};

class PointerType : public DataType {
    const int TYPE_ID = POINTER_TYPE_VALUE;
    const std::shared_ptr<DataType> referenceType;
public:
    explicit PointerType(std::shared_ptr<DataType> referenceType) : referenceType(std::move(referenceType)) {}
    int getTypeId() override {
        return TYPE_ID;
    }
    bool typeEqual(DataType* other) override {
        return TYPE_ID == other->getTypeId() && referenceType->typeEqual(other->getSubType());
    }
    bool isPointer() override {
        return true;
    }
    DataType* getSubType() override {
        return referenceType.get();
    }
};