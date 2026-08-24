#pragma once

#include <memory>
#include <utility>

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
    const int TYPE_ID = 0;
public:
    int getTypeId() override {
        return TYPE_ID;
    }
    bool typeEqual(DataType* other) override {
        return TYPE_ID == other->getTypeId();
    }
};

class IntType : public DataType {
    const int TYPE_ID = 1;
public:
    int getTypeId() override {
        return TYPE_ID;
    }
    bool typeEqual(DataType* other) override {
        return TYPE_ID == other->getTypeId();
    }
};

class FixedType : public DataType {
    const int TYPE_ID = 2;
public:
    int getTypeId() override {
        return TYPE_ID;
    }
    bool typeEqual(DataType* other) override {
        return TYPE_ID == other->getTypeId();
    }
};

class PointerType : public DataType {
    const int TYPE_ID = 3;
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