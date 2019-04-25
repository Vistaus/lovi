/*
 * Copyright 2019 Aurélien Gâteau <mail@agateau.com>
 *
 * This file is part of Lovi.
 *
 * Lovi is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "Searcher.h"

#include "ConditionIO.h"
#include "Conditions.h"
#include "LogFormat.h"
#include "TestUtils.h"

#include <QSignalSpy>

#include <catch2/catch.hpp>

using std::unique_ptr;

class StringListSearchable : public Searchable {
public:
    StringListSearchable(const QStringList& lines) : mLines(lines) {
    }

    int lineCount() const override {
        return mLines.count();
    }

    bool lineMatches(int row, const Condition* condition) const override {
        if (row >= lineCount()) {
            return false;
        }
        return condition->eval(mLines.at(row));
    }

private:
    QStringList mLines;
};

unique_ptr<Condition> createTestCondition(const QString& text) {
    static ColumnHash hash = {{"l", 1}};
    auto conditionOrError = ConditionIO::parse(text, hash);
    REQUIRE(std::holds_alternative<unique_ptr<Condition>>(conditionOrError));
    return std::move(std::get<unique_ptr<Condition>>(conditionOrError));
}

TEST_CASE("Searcher") {
    Searcher searcher;
    QSignalSpy finishedSpy(&searcher, &Searcher::finished);

    StringListSearchable searchable({"foo", "bar", "baz"});

    auto checkFinishedEmitted = [&finishedSpy](const SearchResponse& expected) {
        REQUIRE(finishedSpy.count() == 1);
        QVariantList args = finishedSpy.takeFirst();
        SearchResponse output = args.at(0).value<SearchResponse>();
        REQUIRE(output.result == expected.result);
        REQUIRE(output.row == expected.row);
    };

    SECTION("DirectHit") {
        searcher.start(&searchable, createTestCondition("l ~ ^b"), SearchDirection::Down, 0);
        checkFinishedEmitted({SearchResponse::DirectHit, 1});

        searcher.start(&searchable, createTestCondition("l ~ ^b"), SearchDirection::Down, 2);
        checkFinishedEmitted({SearchResponse::DirectHit, 2});
    }

    SECTION("NoHit") {
        searcher.start(&searchable, createTestCondition("l ~ ^notFound"), SearchDirection::Down, 0);
        checkFinishedEmitted({});
    }

    SECTION("WrappedUp") {
        searcher.start(&searchable, createTestCondition("l ~ ^foo"), SearchDirection::Down, 1);
        checkFinishedEmitted({SearchResponse::WrappedDown, 0});
    }
}