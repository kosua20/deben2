#!/usr/bin/env python

#from __future__ import unicode_literals

import collections
import datetime
import decimal
import sys

from pdfminer.converter import PDFPageAggregator
from pdfminer.layout import LAParams, LTTextBox
from pdfminer.pdfinterp import PDFPageInterpreter, PDFResourceManager
from pdfminer.pdfpage import PDFPage


class CustomConverter(PDFPageAggregator):

    # Horizontal coordinates of vertical lines that delimit columns.
    boundaries = [38, 70, 358, 404, 484, 560]

    def __init__(self, *args, **kwargs):
        laparams = LAParams(char_margin=1.2, line_margin=0.1)
        kwargs.setdefault('laparams', laparams)
        super(CustomConverter, self).__init__(*args, **kwargs)
        # Public variable holding relevant rows when processing is complete.
        self.rows = []

    def receive_layout(self, ltpage):
        """Extract data rows."""
        # Group pieces of text by vertical coordinate:
        rows = collections.defaultdict(lambda: [[] for _ in self.boundaries])
        for item in ltpage:
             # If an item on the page is a text box...
            if isinstance(item, LTTextBox):
                # And it falls inside one of the "boundaries" range...
                #print(str(item.x0) + "," + str(item.x1) + ": " + item.get_text().strip())
                for col in range(len(self.boundaries)-1):
                    left = self.boundaries[col]
                    right = self.boundaries[col+1]
                    if left - 1 < item.x0 < item.x1 < right + 1:
                        #print(str(col) + ": " + item.get_text().strip())
                        # Store it in the corresponding column.
                        # Columns elements are grouped based on their vertical position.
                        rows[int(item.y0)][col].append(item.get_text().strip())
                        break

        # Sort rows.
        sorted_rows = sorted(rows.items(), reverse=True)
        # Merge close rows:
        for (y1, row1), (y2, row2) in zip(sorted_rows[:-1], sorted_rows[1:]):
            # If they are almost vertically aligned (6px arbitrary threshold)...
            if y1 - y2 < 6:
                # Merge their elements column-wise.
                rows[y1] = [item1 + item2 for item1, item2 in zip(row1, row2)]
                # Remove merged row.
                del rows[y2]

        # Process relevant rows and store their data:
        for _, row in sorted(rows.items(), reverse=True):
            row = [' '.join(item) for item in row]
            if not all(row[:3]):
                continue
            # Skip headers.
            #print(row)
            if row == ['DATE', 'LIBELLE', 'VALEUR', 'DEBIT', 'CREDIT', '']:
                continue
            # Extract label, date, amount?
            label = row[1]
            day, month, year = map(int, row[2].split('.'))
            date = datetime.date(2000 + year, month, day)
            amount = '-' + row[3] if ',' in row[3] else row[4]
            amount = decimal.Decimal(amount.replace(' ', '').replace(',', '.'))
            # Store the result.
            self.rows.append((label, date, amount))

    def render_image(self, name, stream):
        """Ignore images."""

    def paint_path(self, gstate, stroke, fill, evenodd, path):
        """Ignore paths."""


# Argument.
if len(sys.argv) < 2:
    print("Usage: extract-lcl.py file-to-extract.pdf")
    exit()

# Files
filePath = sys.argv[1]
fp = open(filePath, "rb")
# We will write the raw result in a txt with the same name as the input file.
fout = open(filePath.replace('.pdf','.txt'), 'w')

# PDF Miner setup
rsrcmgr = PDFResourceManager()
device = CustomConverter(rsrcmgr)
interpreter = PDFPageInterpreter(rsrcmgr, device)

# Extraction
for page in PDFPage.get_pages(fp):
    interpreter.process_page(page)
rows = device.rows

#Write rows to file.
for label, date, amount in rows:
    fout.write("{}  {:+8.2f}  {}\n".format(date, amount, label))
fout.close()
