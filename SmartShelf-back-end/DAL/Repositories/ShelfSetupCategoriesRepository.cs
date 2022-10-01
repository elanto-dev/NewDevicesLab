using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Domain;
using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.ChangeTracking;

namespace DAL.Repositories
{
    public class ShelfSetupCategoriesRepository : BaseRepository
    {
        private readonly AppDbContext _context;

        public ShelfSetupCategoriesRepository(AppDbContext context) : base(context)
        {
            _context = context;
        }

        public async Task<List<ShelfSetupCategories>> GetByShelfInTrip(int shelfInTripId)
        {
            return await _context.ShelfSetupCategories.Where(t => t.ShelfInTripId == shelfInTripId)
                .Include(s => s.Category).AsNoTracking().ToListAsync();
        }

        public async Task<EntityEntry<ShelfSetupCategories>> AddAsync(ShelfSetupCategories shelfSetup)
        {
            return await _context.ShelfSetupCategories.AddAsync(shelfSetup);
        }

        public EntityEntry<ShelfSetupCategories> Update(ShelfSetupCategories shelfSetup)
        {
            return _context.ShelfSetupCategories.Update(shelfSetup);
        }

        public EntityEntry<ShelfSetupCategories> Delete(ShelfSetupCategories shelfSetup)
        {
            return _context.ShelfSetupCategories.Remove(shelfSetup);
        }
    }
}
