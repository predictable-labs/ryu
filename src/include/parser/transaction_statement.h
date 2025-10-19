#pragma once

#include "statement.h"
#include "transaction/transaction_action.h"

namespace ryu {
namespace parser {

class TransactionStatement : public Statement {
    static constexpr common::StatementType statementType_ = common::StatementType::TRANSACTION;

public:
    explicit TransactionStatement(transaction::TransactionAction transactionAction)
        : Statement{statementType_}, transactionAction{transactionAction} {}

    transaction::TransactionAction getTransactionAction() const { return transactionAction; }

private:
    transaction::TransactionAction transactionAction;
};

} // namespace parser
} // namespace ryu
