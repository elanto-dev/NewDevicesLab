using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Domain;
using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.ChangeTracking;

namespace DAL.Repositories
{
    public class ShelfRepository : BaseRepository
    {
        private readonly AppDbContext _context;

        public ShelfRepository(AppDbContext context) : base(context)
        {
            _context = context;
        }

        public async Task<List<Shelf>> GetByUserId(int userId)
        {
            return await _context.Shelves.Where(u => u.UserId == userId).AsNoTracking().ToListAsync();
        }

        public async Task<Shelf> GetById(int id, int userId)
        {
            return await _context.Shelves.AsNoTracking().FirstOrDefaultAsync(u => u.Id == id && u.UserId == userId);
        }

        public async Task<Shelf> GetByIdentifier(string ident, int userId)
        {
            return await _context.Shelves.AsNoTracking().FirstOrDefaultAsync(u => u.Identifier == ident && u.UserId == userId);
        }

        public EntityEntry<Shelf> Update(Shelf shelf)
        {
            return _context.Shelves.Update(shelf);
        }

        public async Task<EntityEntry<Shelf>> AddAsync(Shelf shelf)
        {
            return await _context.Shelves.AddAsync(shelf);
        }

        public EntityEntry<Shelf> Delete(Shelf shelf)
        {
            return _context.Shelves.Remove(shelf);
        }

        public async Task<bool> Exists(int id)
        {
            return await _context.Shelves.AnyAsync(s => s.Id == id);
        }
    }
}
