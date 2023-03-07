# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

# -*- coding: utf-8 -*-
from gitlint.rules import CommitRule, RuleViolation
from gitlint.options import StrOption


class BodyInvalidWords(CommitRule):
    """This rule will check that the commit message does not contain any
    of the designated banned words.
    """

    # Human friendly name for the rule
    name = "body-invalid-words"

    # Unique id for User-defined Commit-rule.
    id = "UC1"

    options_spec = [StrOption("invalid-words", "", "List of banned words")]

    def validate(self, commit):
        # Pre-defined list of invalid / banned words
        banned_words_list = (self.options["invalid-words"].value).split(":")
        banned_words = set([x.upper() for x in banned_words_list])

        violation = ""

        # Check the commit message
        commit_message_list = (str(commit.message.body)).split()
        commit_message = set([x.upper() for x in commit_message_list])

        matches = banned_words.intersection(commit_message)
        if len(matches) > 0:
            violation += (
                "Commit message contains the following banned word(s): "
                + ",".join(matches)
                + "\n"
            )

        # Check the commit title
        commit_title = set((str(commit.message.title)).split())
        matches = banned_words.intersection(commit_title)
        if len(matches) > 0:
            violation += (
                "Commit title contains the following banned word(s): "
                + ",".join(matches)
                + "\n"
            )

        if len(violation) > 0:
            return [RuleViolation(self.id, violation)]
        return []
