#
# @file     scripts/utility/wordlist.py
# @brief    Python module defining the WordList class.
#
# @author   Chris Vig (chris@invictus.so)
# @date     2025-09-10
# @cpyrt    © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
#

# ------------------------------------------------------ IMPORTS -------------------------------------------------------

import random
from urllib import request

# ------------------------------------------------------ EXPORTS -------------------------------------------------------

__all__ = [
    'WordList'
]

# ----------------------------------------------------- CONSTANTS ------------------------------------------------------

DEFAULT_WORD_LIST_URL = 'https://raw.githubusercontent.com/first20hours/google-10000-english/refs/heads/master/google-10000-english-usa-no-swears.txt'

# ------------------------------------------------------- TYPES --------------------------------------------------------

class WordList:
    """
    Class providing access to a list of words which may be selected from randomly.
    """
    def __init__(self, url: str = DEFAULT_WORD_LIST_URL):
        """
        Initializes a new instance using the file at the specified URL.
        """
        # Load file from URL
        self.url = url
        with request.urlopen(url) as response:
            data = response.read()

        # Get word list by splitting up lines
        self.words = str(data, encoding='utf8').splitlines()

        # Get word list sorted by length and determine minimum and maximum
        self.words_by_length = sorted(self.words, key=len)
        self.min_length = len(self.words_by_length[0])
        self.max_length = len(self.words_by_length[-1])

        # Find start index for each length
        self.words_by_length_indices = { }
        for idx, word in enumerate(self.words_by_length):
            for length in range(self.min_length, len(word) + 1):
                if length not in self.words_by_length_indices:
                    self.words_by_length_indices[length] = idx

    def __len__(self):
        """
        Returns the number of words in this word list.
        """
        return len(self.words)

    def _start_idx(self, length: int) -> int:
        """
        Returns the start index in the sorted list for words with the specified length.
        """
        if length < self.min_length:
            return 0
        elif length > self.max_length:
            return len(self)
        else:
            return self.words_by_length_indices[length]

    def random(self, min_length: int = None, max_length: int = None):
        """
        Returns a randomly selected word from this word list.
        """
        # Sanity check range
        if min_length is None or min_length < self.min_length:
            min_length = self.min_length
        if max_length is None or max_length > self.max_length:
            max_length = self.max_length

        # Get indices in the sorted list for the requested lengths
        start_idx = self._start_idx(min_length)
        end_idx = self._start_idx(max_length + 1)
        return self.words_by_length[random.randrange(start_idx, end_idx)]
