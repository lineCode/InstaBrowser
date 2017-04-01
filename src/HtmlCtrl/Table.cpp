#include "Html.h"
#include "Table.h"
#include "HtmlTag.h"

NAMESPACE_HTMLCTRL

void TableGrid::AddCell( Element* el )
{
	TableCell cell;
	cell.el = el;
	cell.colspan	= StrInt(el->GetAttr("colspan", "1"));
	cell.rowspan	= StrInt(el->GetAttr("rowspan", "1"));
	cell.Borders	= el->GetBorders();

	while( IsRowSpanned( (int) m_cells.GetCount() - 1, (int) LAST(m_cells).GetCount() ) ) {
		LAST(m_cells).Add(TableCell());
	}

	LAST(m_cells).Add(cell);
	for(int i = 1; i < cell.colspan; i++)
	{
		TableCell empty_cell;
		LAST(m_cells).Add(empty_cell);
	}
}


void TableGrid::BeginRow(Element* row)
{
	Vector<TableCell> r;
	m_cells.Add(r);
	
	m_rows.Add(TableRow(0, row));

}


bool TableGrid::IsRowSpanned( int r, int c )
{
	for(int row = r - 1; row >= 0; row--)
	{
		if(c < (int) m_cells[row].GetCount())
		{
			if(m_cells[row][c].rowspan > 1)
			{
				if(m_cells[row][c].rowspan >= r - row + 1)
				{
					return true;
				}
			}
		}
	}
	return false;
}

void TableGrid::Finish()
{
	m_RowsCount	= (int) m_cells.GetCount();
	m_ColsCount	= 0;
	for(int i = 0; i < (int) m_cells.GetCount(); i++)
	{
		m_ColsCount = max(m_ColsCount, (int) m_cells[i].GetCount());
	}
	for(int i = 0; i < (int) m_cells.GetCount(); i++)
	{
		for(int j = (int) m_cells[i].GetCount(); j < m_ColsCount; j++)
		{
			TableCell empty_cell;
			m_cells[i].Add(empty_cell);
		}
	}

	m_columns.Clear();
	for(int i = 0; i < m_ColsCount; i++)
	{
		m_columns.Add(TableColumn(0, 0));
	}

	for(int col = 0; col < m_ColsCount; col++)
	{
		for(int row = 0; row < m_RowsCount; row++)
		{
			if(Cell(col, row)->el)
			{
				// find minimum left Border width
				if(m_columns[col].border_left)
				{
					m_columns[col].border_left = min(m_columns[col].border_left, Cell(col, row)->Borders.left);
				} else
				{
					m_columns[col].border_left = Cell(col, row)->Borders.left;
				}
				// find minimum right Border width
				if(m_columns[col].border_right)
				{
					m_columns[col].border_right = min(m_columns[col].border_right, Cell(col, row)->Borders.right);
				} else
				{
					m_columns[col].border_right = Cell(col, row)->Borders.right;
				}
				// find minimum top Border width
				if(m_rows[row].border_top)
				{
					m_rows[row].border_top = min(m_rows[row].border_top, Cell(col, row)->Borders.top);
				} else
				{
					m_rows[row].border_top = Cell(col, row)->Borders.top;
				}
				// find minimum bottom Border width
				if(m_rows[row].border_bottom)
				{
					m_rows[row].border_bottom = min(m_rows[row].border_bottom, Cell(col, row)->Borders.bottom);
				} else
				{
					m_rows[row].border_bottom = Cell(col, row)->Borders.bottom;
				}
			}

			if(Cell(col, row)->el && Cell(col, row)->colspan <= 1)
			{
				if(!Cell(col, row)->el->GetCssWidth().IsPredefined())
				{
					m_columns[col].css_width = Cell(col, row)->el->GetCssWidth();
					break;
				}
			}
		}
	}

	for(int col = 0; col < m_ColsCount; col++)
	{
		for(int row = 0; row < m_RowsCount; row++)
		{
			if(Cell(col, row)->el)
			{
				Cell(col, row)->el->SetCssWidth(m_columns[col].css_width);
			}
		}
	}
}

TableCell* TableGrid::Cell( int t_col, int t_row )
{
	if(t_col >= 0 && t_col < m_ColsCount && t_row >= 0 && t_row < m_RowsCount)
	{
		return &m_cells[t_row][t_col];
	}
	return 0;
}

void TableGrid::DistributeMaxWidth( int width, int start, int end )
{
	TableColumnAccessorMaxWidth Selector;
	DistributeWidth(width, start, end, &Selector);
}

void TableGrid::DistributeMinWidth( int width, int start, int end )
{
	TableColumnAccessorMinWidth Selector;
	DistributeWidth(width, start, end, &Selector);
}

void TableGrid::DistributeWidth( int width, int start, int end, TableColumnAccessor* acc )
{
	if(!(start >= 0 && start < m_ColsCount && end >= 0 && end < m_ColsCount))
	{
		return;
	}

	int cols_width = 0;
	for(int col = start; col <= end; col++)
	{
		cols_width		+= m_columns[col].max_width;
	}

	int add = width / (end - start + 1);
	int added_width = 0;
	for(int col = start; col <= end; col++)
	{
		if(cols_width)
		{
			add = RoundFlt( (float) width * ((float) m_columns[col].max_width / (float) cols_width) );
		}
		added_width += add;
		acc->get(m_columns[col]) += add;
	}
	if(added_width < width)
	{
		acc->get(m_columns[start]) += width - added_width;
	}
}

void TableGrid::DistributeWidth( int width, int start, int end )
{
	if(!(start >= 0 && start < m_ColsCount && end >= 0 && end < m_ColsCount))
	{
		return;
	}

	Vector<TableColumn*> distribute_columns;

	for(int step = 0; step < 3; step++)
	{
		distribute_columns.Clear();

		switch(step)
		{
		case 0:
			{
				// distribute between the columns with width == auto
				for(int col = start; col <= end; col++)
				{
					if(m_columns[col].css_width.IsPredefined())
					{
						distribute_columns.Add(&m_columns[col]);
					}
				}
			}
			break;
		case 1:
			{
				// distribute between the columns with percents
				for(int col = start; col <= end; col++)
				{
					if(!m_columns[col].css_width.IsPredefined() && m_columns[col].css_width.Units() == css_units_percentage)
					{
						distribute_columns.Add(&m_columns[col]);
					}
				}
			}
			break;
		case 2:
			{
				//well distribute between all columns
				for(int col = start; col <= end; col++)
				{
					distribute_columns.Add(&m_columns[col]);
				}
			}
			break;
		}

		int added_width = 0;

		if(!distribute_columns.IsEmpty() || step == 2)
		{
			int cols_width = 0;
			for(Vector<TableColumn*>::Iterator col = distribute_columns.Begin(); col != distribute_columns.End(); col++)
			{
				cols_width += (*col)->max_width - (*col)->min_width;
			}

			if(cols_width)
			{
				int add = width / (int) distribute_columns.GetCount();
				for(Vector<TableColumn*>::Iterator col = distribute_columns.Begin(); col != distribute_columns.End(); col++)
				{
					add = RoundFlt( (float) width * ((float) ((*col)->max_width - (*col)->min_width) / (float) cols_width) );
					if((*col)->width + add >= (*col)->min_width)
					{
						(*col)->width	+= add;
						added_width		+= add;
					} else
					{
						added_width	+= ((*col)->width - (*col)->min_width) * (add / abs(add));
						(*col)->width = (*col)->min_width;
					}
				}
				if(added_width < width && step)
				{
					distribute_columns[0]->width += width - added_width;
					added_width = width;
				}
			} else
			{
				LAST(distribute_columns)->width += width;
				added_width = width;
			}
		}

		if(added_width == width)
		{
			break;
		} else
		{
			width -= added_width;
		}
	}
}

int TableGrid::CalcTableWidth(int block_width, bool is_auto, int& min_table_width, int& max_table_width)
{
	//int table_width = 0;

	min_table_width = 0; // MIN
	max_table_width = 0; // MAX

	int cur_width = 0;
	int max_w = 0;
	int min_w = 0;

	for(int col = 0; col < m_ColsCount; col++)
	{
		min_table_width += m_columns[col].min_width;
		max_table_width += m_columns[col].max_width;

		if(!m_columns[col].css_width.IsPredefined())
		{
			m_columns[col].width = m_columns[col].css_width.CalcPercent(block_width);
			m_columns[col].width = max(m_columns[col].width, m_columns[col].min_width);
		} else
		{
			m_columns[col].width = m_columns[col].min_width;
			max_w += m_columns[col].max_width;
			min_w += m_columns[col].min_width;
		}

		cur_width += m_columns[col].width;
	}

	if(cur_width == block_width)
	{
		return cur_width;
	}

	if(cur_width < block_width)
	{
		if(cur_width - min_w + max_w <= block_width)
		{
			cur_width = 0;
			for(int col = 0; col < m_ColsCount; col++)
			{
				if(m_columns[col].css_width.IsPredefined())
				{
					m_columns[col].width = m_columns[col].max_width;
				}
				cur_width += m_columns[col].width;
			}
			if(cur_width == block_width || is_auto)
			{
				return cur_width;
			}
		}
		DistributeWidth(block_width - cur_width, 0, m_ColsCount - 1);
		cur_width = 0;
		for(int col = 0; col < m_ColsCount; col++)
		{
			cur_width += m_columns[col].width;
		}
	} else
	{
		int fixed_width = 0;
		float percent = 0;
		for(int col = 0; col < m_ColsCount; col++)
		{
			if(!m_columns[col].css_width.IsPredefined() && m_columns[col].css_width.Units() == css_units_percentage)
			{
				percent += m_columns[col].css_width.Value();
			} else
			{
				fixed_width += m_columns[col].width;
			}
		}
		float scale = (float) (100.0 / percent);
		cur_width = 0;
		for(int col = 0; col < m_ColsCount; col++)
		{
			if(!m_columns[col].css_width.IsPredefined() && m_columns[col].css_width.Units() == css_units_percentage)
			{
				CssLength w;
				w.SetValue(m_columns[col].css_width.Value() * scale, css_units_percentage);
				m_columns[col].width = w.CalcPercent(block_width - fixed_width);
				if(m_columns[col].width < m_columns[col].min_width)
				{
					m_columns[col].width = m_columns[col].min_width;
				}
			}
			cur_width += m_columns[col].width;
		}
	}
	return cur_width;
}

void TableGrid::Clear()
{
	m_RowsCount	= 0;
	m_ColsCount	= 0;
	m_cells.Clear();
	m_columns.Clear();
	m_rows.Clear();
}

void TableGrid::CalcHorizPos( Margins& table_borders, BorderCollapse bc, int bdr_space_x)
{
	if(bc == border_collapse_separate)
	{
		int left = bdr_space_x;
		for(int i = 0; i < m_ColsCount; i++)
		{
			m_columns[i].left	= left;
			m_columns[i].right	= m_columns[i].left + m_columns[i].width;
			left = m_columns[i].right + bdr_space_x;
		}
	} else
	{
		int left = 0;
		if(m_ColsCount)
		{
			left -= min(table_borders.left, m_columns[0].border_left);
		}
		for(int i = 0; i < m_ColsCount; i++)
		{
			if(i > 0)
			{
				left -= min(m_columns[i - 1].border_right, m_columns[i].border_left);
			}

			m_columns[i].left	= left;
			m_columns[i].right	= m_columns[i].left + m_columns[i].width;
			left = m_columns[i].right;
		}
	}
}

void TableGrid::CalcVertPos( Margins& table_borders, BorderCollapse bc, int bdr_space_y )
{
	if(bc == border_collapse_separate)
	{
		int top = bdr_space_y;
		for(int i = 0; i < m_RowsCount; i++)
		{
			m_rows[i].top		= top;
			m_rows[i].bottom	= m_rows[i].top + m_rows[i].height;
			top = m_rows[i].bottom + bdr_space_y;
		}
	} else
	{
		int top = 0;
		if(m_RowsCount)
		{
			top -= min(table_borders.top, m_rows[0].border_top);
		}
		for(int i = 0; i < m_RowsCount; i++)
		{
			if(i > 0)
			{
				top -= min(m_rows[i - 1].border_bottom, m_rows[i].border_top);
			}

			m_rows[i].top		= top;
			m_rows[i].bottom	= m_rows[i].top + m_rows[i].height;
			top = m_rows[i].bottom;
		}
	}
}


int& TableColumnAccessorMaxWidth::get( TableColumn& col )
{
	return col.max_width;
}

int& TableColumnAccessorMinWidth::get( TableColumn& col )
{
	return col.min_width;
}

int& TableColumnAccessorWidth::get( TableColumn& col )
{
	return col.width;
}

END_HTMLCTRL_NAMESPACE
