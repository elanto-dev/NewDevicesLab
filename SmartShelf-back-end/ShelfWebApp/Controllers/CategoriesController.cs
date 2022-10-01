using System.Collections.Generic;
using System.Threading.Tasks;
using DAL;
using DAL.Repositories;
using Domain;
using Microsoft.AspNetCore.Mvc;

namespace ShelfWebApp.Controllers
{
    [Route("api/[controller]")]
    [ApiController]
    //[Authorize(AuthenticationSchemes = JwtBearerDefaults.AuthenticationScheme)]
    public class CategoriesController : ControllerBase
    {
        private readonly CategoryRepository _categoryRepository;

        public CategoriesController(AppDbContext context)
        {
            _categoryRepository = new CategoryRepository(context);
        }

        // GET: api/Categories/5
        [HttpGet("{id}")]
        public async Task<ActionResult<Category>> GetCategory(int id)
        {
            var category = await _categoryRepository.GetById(id);

            if (category == null)
            {
                return NotFound();
            }

            return category;
        }

        // GET: api/Categories
        [HttpGet]
        public async Task<ActionResult<IEnumerable<Category>>> GetCategories()
        {
            var categories = await _categoryRepository.GetAll();

            if (categories == null || categories.Count == 0)
            {
                return NotFound();
            }

            return categories;
        }
    }
}
